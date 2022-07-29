#include <Platform/IA32.h>
#include <Platform/8259.h>
#include <Platform/X87.h>
#include <Intr_Trap.h>
#include <Atomic.h>

// Note that DPMI is called with INT 3Ch
#define IDT_SIZE 256

// Includes two IO bitmaps
CompleteTSS main_tss;

static qword gdt[GDT_ENTRIES] = {
    [GDT_KCODE] = GDTDEF_R0_CSEG,
    [GDT_KDATA] = GDTDEF_R0_DSEG,
    [GDT_UCODE] = GDTDEF_R3_CSEG,
    [GDT_UDATA] = GDTDEF_R3_DSEG,
    [GDT_TSSD]  = GDTDEF_R0_TSS
};

static Intd idt[IDT_SIZE];

// Ignore not used warnings, used in StartK.asm
xDtr gdtr = {.limit=sizeof(gdt) -1, .address=(dword)&gdt};
xDtr idtr = {.limit=(IDT_SIZE*8)-1, .address=(dword)&idt};

char test = '!';

// Table of exception vectors
static void (*except[EXCEPT_IMPLEMENTED])() =
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

static inline void FillIDT(void)
{
    // 15 exceptions, 16 IRQs? Think about it.
    int i,j;
    for (i=0;i<EXCEPT_IMPLEMENTED;i++)
        MkTrapGate(idt,i,0,except[i]);
    for (i=0;i<16;i++)
        MkIntrGate(idt, EXCEPT_IMPLEMENTED + i, &Low0 + i*4);
}

// Compiler is really bad at optimizing this so I did it manually
// This function does not modify the rest of the GDT entry
// and only touches the address fields
//
static inline void AppendAddress(pvoid gdt_entry, dword address)
{
    __asm__ volatile (
    "mov    $8,     %%cl"  ASNL
    "mov    %0,     %%eax" ASNL
    "mov    %1,     %%ebx" ASNL
    "mov    %%ax,   2(%%ebx)" ASNL
    "shr    %%cl,   %%eax"    ASNL
    "mov    %%al,   3(%%ebx)" ASNL
    "shr    %%cl,   %%eax"    ASNL
    "mov    %%ah,   7(%%ebx)" ASNL
    :
    :"r"(address),"r"(gdt_entry)
    :"ebx","eax","flags","memory"
    );
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
    byte icw1 = ICW1 | ICW1_ICW4;

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

/// @brief Detect the X87 and set it up if present
/// Read Resource.c for information on IRQ#13
/// @return
/// @retval 0 No FPU present
///
static Status SetupX87(void)
{
    // The Native Exceptions bit, when set, the FPU
    // exception is sent to the dedicated vector
    // otherwise, an IRQ is sent. IRQ#13 is hardwired

    dword cr0;
    __asm__ volatile ("mov %%cr0, %0":"=r"(cr0)::"memory");

    //
    // If the EM bit is turned on at startup it
    // is assumed that the FPU is not meant to be used
    //
    if (BIT_IS_SET(cr0, CR0_EM))
        return 0;

    // If present the
}

void InitIA32(void)
{
    //
    // SET UP TASK STATE SEGMENT
    //
    AppendAddress(&gdt[GDT_TSSD], (dword)&main_tss);
    __asm__ volatile ("ltr %%ax" : : "ax"(GDT_TSSD<<3) : "memory");

    C_memset(&main_tss.iopb_deny_all, '\xFF', 0x2000);

    //
    // CONFIGURE PIC
    //
    PIC_Remap();

    pic_outb(0x20,0xB);  // Tell the PICs to send the ISR through CMD port reads
    pic_outb(0xA0,0xB);  // The kernel does need the IRR so this improves performance

    // Mask all interrupts, writes to data port is OCW3/IMR
    pic_outb(0xA1,0xFF);
    pic_outb(0x21,0xFF);

    //
    // POPULATE THE INTERRUPT DESCRIPTOR TABLE
    //
    FillIDT();
}
