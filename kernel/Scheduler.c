/*
     This file is part of OS/90.

    OS/90 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

    OS/90 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with OS/90. If not, see <https://www.gnu.org/licenses/>. 
*/


// Note: Variables shared by ISRs and kernel must be volatile because
// they can change unpredictably

// The FPU registers only need to be saved when
// another process tries to use them
// Scheduler priority for FPU switch? No.
//

// Perhaps add interrupt priority based on masking?

#include <Platform/8259.h>
#include <Platform/IA32.h>
#include <Scheduler.h>
#include <Farcall.h>
#include <PnP_Mgr.h>
#include <Type.h>
#include <V86.h>

// used by vm86.asm, automatically cleared
// by the gpf handler when it is set by VM86.asm
BOOL  vm86_caused_gpf=0; // ?
DWORD current_proc=0;

// The mode that the PC switched from
// There are three modes:
// * Interrupt Service Routine (IRQ)
// * Kernel
// * User
// Software interrupts, or traps, are KERNEL
//
// Initialzed to KERNEL because that would be correct when
// the first interrupt is called.
//
// This matters because interrupt stack frames are different if switched
// from a userspace program (SS and ESP are saved)
//
static INTVAR BYTE last_mode = KERNEL;

// Counts the number of spurious interrupts
static INTVAR DWORD spurious_interrupts = 0;

// Fixed point number for countine miliseconds the OS has been running
// Not currently being used for anything though
//
static INTVAR QWORD uptime = 0; // Fixed point

void Divide0()
{}

void Debug()
{}

void NMI()
{}

void Breakpoint()
{}

void Overflow()
{}

void BoundRangeExceeded()
{}

void InvalidOp()
{}

void DevNotAvail()
{}

void DoubleFault()
{}

void SegOverrun()
{}

void InvalidTSS()
{}

void SegNotPresent()
{}

void StackSegFault()
{}

void FloatError()
{}

void PageFault(PTRAP_FRAME tf)
{
    DWORD error_code = ScGetFaultErrorCode();
    DWORD pfla; __asm__("movl $cr0, %0" :"=r"(pfla)::);

    const BOOL caused_by_cpl_3 = (error_code >> 2)&1;

    // For this to be a far call hook, the following conditions must be met
    // * Code segment must be FAR_CALL_BASE_SEG
    // * Fault must be caused by a ring three program (V86 is always ring 3)
    // * Fault address must be in appropriante range withing segment
    if (       pfla >= FAR_CALL_BASE_ADDR
            && pfla <= FAR_CALL_BASE_ADDR+FAR_CALL_UPPER_BOUND_OFFSET)
            && caused_by_cpl_3
            && tf->cs == FAR_CALL_BASE_SEG
    {
        // This page fault is a call by a DOS program to a 32-bit far call
        // handler. Dispatch to Farcall.c.
        HandleFcallAfterPF(tf->eip, tf);

        // We are done here, continue execution.
        return;
    }
}

BOOL ScCurrentProgramInProtectedMode();

VOID GeneralProtect(PTRAP_FRAME tf)
{
    /*
     * The error code is always zero if it is not
     * segment related. There are no zero selectors
    */

    //
    // If a DPMI app calls a vector that is not ring 3, 
    //


   // Is it from the kernel?
   // If so PANIC

    // Is the GPF from VM86?

    // If not, terminate the current task
}

static inline void SendEOI(BYTE vector)
{
    pic_outb(0x20, 0x20);
    if (vector > 7)
        pic_outb(0xA1, 0x20);
}

//
// IRQ#0 has two functions, update the uptime and switch tasks.
// RIght now, task switching is done every 1MS, which means every time
// IRQ#0 is handled.
//
// Interrrupts are off when handling IRQ#0
//
static VOID HandleIRQ0(IN PTRAP_FRAME t)
{
    static BYTE time_slice_in_progress;
    static WORD ms_left;

    uptime += 0x10000000; // Trust me bro
    // Update the DOS/BIOS time in BDA?
}

//
// The master interrupt dispatcher and ISR of all interrupts.
// EAX and EDX pass the arguments for simplicity
//
__attribute__(( regparm(2), optimize("align-functions=64") ))
VOID InMasterDispatch(IN PTRAP_FRAME tf, DWORD irq)
{
    WORD  inservice16 = InGetInService16();

    /// 1. The ISR is set to zero for both PICs upon SpurInt.
    /// 2. If an spurious IRQ comes from master, no EOI is sent
    /// because there is no IRQ. if it is from the slave PIC
    /// EOI is sent to the master only

    last_mode = INTERRUPT;

    // Is this IRQ#0? If so, handle it directly
    if (BIT_IS_SET(inservice16, 0))
        HandleIRQ0(tf);

    // Is this a spurious IRQ? If so, do not handle.
    if (irq == 7 && (inservice16 & 0xFF) == 0)
        goto CountAndRet; // (-_-)
    else if (irq == 15 && (inservice16 >> 8) == 0)
    {
        SendEOI(0); // Send to master in case of spurious 15
        CountAndRet:
            spurious_interrupts++;
            return;
    }

    if (InGetInterruptLevel(irq) == BUS_INUSE)
    {
        // Interrupts can fire during an ISR if there is nothing atomic going on
        IntsOn();
        InGetInterruptHandler(irq)(tf);
        SendEOI(irq);
    }
    else if (RECL_16)
    {
        // Disable interrupts
        // EOI will be sent by the ISR
        // But what will this actually do?
    } else {
        // Critical error
    }
}

//
// DPMI has a ridiculous requirement where all protected mode
// interrupt vectors must point to code that reflects it to DOS,
// with the exception of INT 21H AH=4CH and INT 31H (DPMI)
// To make this work, I have to make sure that each IDT entry that is not an IRQ
// is a ring zero vector that points to nothing important.
// This will generate a #GP
//
VOID Init_DPMI_ReflectionHandlers()
{
}

VOID CheckAndHandleDPMI_Trap()
{}

VOID InitScheduler()
{

    // Claim the timer IRQ resource
    // The handler is called by dispatcher directly for speed

    // Now get IRQ#13 and assign the ISR
    // IntrRequestFixed

    //
    // Each interrupt has its own vector so that the IRQ number
    // can be automatically deduced in the case of a spurious interrupt
    // Index the ISRs since they are same size?

    // The exception handlers are already installed by IA32.c

}
