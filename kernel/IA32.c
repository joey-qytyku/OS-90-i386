/*
     This file is part of OS/90.

    OS/90 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

    OS/90 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with OS/90. If not, see <https://www.gnu.org/licenses/>.
*//*

2022-07-08 - Refactoring

*/

#include <Platform/IA32.h>
#include <Platform/8259.h>
#include <Platform/X87.h>
#include <Intr_Trap.h>
#include <Atomic.h>
#include <Type.h>
#include <Debug.h>

// Note that DPMI is called with INT 31h
#define IDT_SIZE 256

// Make this... volatile? No.
IA32_STRUCT _ia32_struct =
{
    .gdt =
    {
    [GDT_NULL]  =   {0},
    [GDT_KCODE] =   {0xFFFF, 0, 0, ACCESS_RIGHTS(1,0,TYPE_CODE), 0xCF, 0x00},
    [GDT_KDATA] =   {0xFFFF, 0, 0, ACCESS_RIGHTS(1,0,TYPE_DATA), 0xCF, 0x00},
    [GDT_UCODE] =   {0xFFFF, 0, 0, ACCESS_RIGHTS(1,3,TYPE_CODE), 0xCF, 0x00},
    [GDT_UDATA] =   {0xFFFF, 0, 0, ACCESS_RIGHTS(1,3,TYPE_DATA), 0xCF, 0x00},
    [GDT_TSSD ]={
     sizeof(COMPLETE_TSS)-1, 0, 0, ACCESS_RIGHTS(1,3,TYPE_TSS),  0x00, 0x00},
    [GDT_LDT] =     {LDT_SIZE-1,0,0, ACCESS_RIGHTS(1,0,TYPE_LDT)},
    [GDT_PNPCS] =   {0xFFFF, 0, 0, ACCESS_RIGHTS(1,0,TYPE_CODE), 0x00, 0x00},
    [GDT_PNP_BIOS_DS]=
                    {0xFFFF, 0, 0, ACCESS_RIGHTS(1,0,TYPE_DATA), 0x00, 0x00},
    [GDT_PNP_OS_DS] =
                    {0xFFFF, 0, 0, ACCESS_RIGHTS(1,0,TYPE_DATA), 0x00, 0x00}
    }
};
// LDT descriptor needs further processing

// Ignore not used warnings, used in StartK.asm
DESCRIPTOR_REGISTER
    gdtr = {(WORD)sizeof(_ia32_struct.gdt)-1, (DWORD)&_ia32_struct.gdt},
    idtr = {256*8-1,    (DWORD)&_ia32_struct.idt};

// Table of exception vectors
static VOID (*except[EXCEPT_IMPLEMENTED])() =
{
    LowDivide0,
    LowDebug,
    LowNMI,
    LowBreakpoint,
    LowOverflow,
    LowBoundRangeExceeded,
    LowInvalidOp,
    LowDevNotAvail,
    LowDoubleFault,
    LowSegOverrun,
    LowInvalidTSS,
    LowSegNotPresent,
    LowStackSegFault,
    LowGeneralProtect,
    LowPageFault
};

//
//
static inline VOID FillIDT(void)
{
    int i;
    for (i=0; i < EXCEPT_IMPLEMENTED; i++)
        MkTrapGate(i, 0, except[i]);

    MkIntrGate(IRQ_BASE, Low0);    MkIntrGate(IRQ_BASE, Low1);
    MkIntrGate(IRQ_BASE, Low2);    MkIntrGate(IRQ_BASE, Low3);
    MkIntrGate(IRQ_BASE, Low4);    MkIntrGate(IRQ_BASE, Low5);
    MkIntrGate(IRQ_BASE, Low6);    MkIntrGate(IRQ_BASE, Low7);
    MkIntrGate(IRQ_BASE, Low8);    MkIntrGate(IRQ_BASE, Low9);
    MkIntrGate(IRQ_BASE, Low10);   MkIntrGate(IRQ_BASE, Low11);
    MkIntrGate(IRQ_BASE, Low12);   MkIntrGate(IRQ_BASE, Low13);
    MkIntrGate(IRQ_BASE, Low14);   MkIntrGate(IRQ_BASE, Low15);
}

// @brief Reprogram the PICs
// @section NOTES
// Different bits tell OCWs and ICWs appart in CMD port
// Industry standard architecture uses edge triggered interrupts
// 8-byte interrupt vectors are default (ICW[:2] = 0)
// 8259.h contains info on IOWAIT and the IRQ_BASE
//
static void PIC_Remap(void)
{
    BYTE icw1 = ICW1 | ICW1_ICW4;

    // ICW1 to both PIC's
    pic_outb(0x20, icw1);
    pic_outb(0xA0, icw1);

    // ICW2, set interrupt vectors
    pic_outb(0x21, IRQ_BASE);
    pic_outb(0xA1, IRQ_BASE+8);

    // ICW3, set cascade
    pic_outb(0x21, 4);  // ICW3, IRQ 2 is cascade (bitmap)
    pic_outb(0xA1, 2);  // ICW3 is different for slave PIC (index)

    pic_outb(0x21, ICW4_8086);
    pic_outb(0xA1, ICW4_8086 | ICW4_SLAVE); // Assert PIC2 is slave
}

//
// Detect the X87 and set it up if present
//
static STATUS SetupX87(VOID)
{
    // The Native Exceptions bit, when set, the FPU
    // exception is sent to the dedicated vector
    // otherwise, an IRQ is sent. IRQ#13 is hardwired

    DWORD cr0;
    __asm__ volatile ("mov %%cr0, %0":"=r"(cr0)::"memory");

    // If the EM bit is turned on at startup it
    // is assumed that the FPU is not meant to be used

    BOOL fpu_not_present = cr0 & CR0_EM != 0;

    // The 80287 did not native exceptions (obviously), so the NE bit
    // should not be set if that is installed

    BOOL using_80387_or_better = cr0 & CR0_ET != 0;

    if (fpu_not_present)
        return 0;

    if (using_80387_or_better)
    {
        // Are native exceptions supported?
        // If so, enable and mask IRQ#13
    }
}

VOID DummyOutDrv(BYTE ch)
{
    static WORD i = 1;
    const PBYTE txt = 0xB8000;
    txt[i] = ch;
    i+=2;
}

void InitIA32(void)
{
    WORD ldt_selector = GDT_LDT<<3;
    WORD tss_selector = GDT_TSSD<<3;

    AppendAddress(&_ia32_struct.gdt[GDT_TSSD], (DWORD)&_ia32_struct.tss);
    __asm__ volatile (
        "ltr %0"
        :
        :"rm"(tss_selector)
        :"memory"
    );
    AppendAddress(&_ia32_struct.gdt[GDT_LDT], (DWORD)&_ia32_struct.ldt);
    __asm__ volatile (
        "lldt %0"
        :
        :"rm"(ldt_selector)
        :"memory"
        );
    return;

    // There are two IO permission bitmaps for all processes
    // One is deny all ports and the other is allow all
    // Set the deny one to all bits on (deny)
    C_memset(&_ia32_struct.tss.iopb_deny_all, '\xFF', 0x2000);

    PIC_Remap();

    pic_outb(0x20,0xB);  // Tell the PICs to send the ISR through CMD port reads
    pic_outb(0xA0,0xB);  // The kernel does need the IRR

    //
    // The PIC will not have interrupts masked. By default, any IRQs
    // not in use by DOS software will already be masked.
    //

    FillIDT();
    KeLogf(DummyOutDrv, "Hello, world!");
}
