/*
     This file is part of OS/90.

    OS/90 is free software: you can redistribute it and/or modify it under the
    terms of the GNU General Public License as published by the Free Software
    Foundation, either version 2 of the License, or (at your option) any later
    version.

    OS/90 is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with OS/90. If not, see <https://www.gnu.org/licenses/>.
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

static DWORD current_proc=0; // ?

//
// Pointer to the current process control block.
//
INTVAR PTHREAD current_pcb;
INTVAR PTHREAD first_pcb; // The first process


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

VOID ScNukeCurrentProcess(VOID)
{}

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

//
// Need to work on this. There may be problems. What if an interrupt handler
// is in the unmapped/ring 0 page? This could work, but I would have to modify
// a page table, which violates abstraction.
//
// Another way is to make the base segment not F000, that way the BIOS is
// not bothered at all.
//
//
CREATE_EXCEPT_HANDLER(PageFault)(PDWORD regs)
{
    const DWORD     error_code      = ScGetFaultErrorCode();
    const BOOL      caused_by_cpl_3 = (error_code >> 2) & 1;
    DWORD pfla;

    __asm__("movl %%cr0, %0" :"=r"(pfla)::);
}

//
// Must be called before calling a virtual software INT. It sets
// SS and ESP before V86 is entered.
//
// When the kernel calls a DOS interrupt vector, a stack must be provided.
//
// The kernel has 32-bit stacks for each process (set in the TSS). Calling
// real mode software requires a stack too.
//
// Lets do that.
//
// The kernel may need to call software interrupts when the scheduler has not
// been yet initialized and no programs are running (with their own RM stacks).
//
// If this is the case, this function will detect if the scheduler has enabled
//
//
//
VOID ScInitDosCallTrapFrame(PDWORD regs)
{
    regs[RD_ESP] = current_pcb->kernel_real_mode_ss;
    regs[RD_SS]  = current_pcb->kernel_real_mode_sp;
}

DWORD ScCurrentProgramInProtectedMode(VOID)
{
    return current_pcb->thread32;
}

CREATE_EXCEPT_HANDLER(GeneralProtect)(DWORD vm, PDWORD regs)
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

    // Is the GPF from V86?
    if (vm)
    {
        ScMonitorV86(regs);
    }

    // If not, terminate the current task
    ScNukeCurrentProcess();
}

static inline void SendEOI(BYTE vector)
{
    pic_outb(0x20, 0x20);
    if (vector > 7)
        pic_outb(0xA1, 0x20);
}

//
// IRQ#0 has two functions, update the uptime and switch tasks.
// Right now, task switching is done every 1MS, which means every time
// IRQ#0 is handled.
//
// This is handled directly. We do not use the regular IRQ handler type.
//
// Interrrupts stay off when handling IRQ#0
//
static VOID HandleIRQ0(DWORD vm, PDWORD t)
{
    static BYTE time_slice_in_progress;
    static WORD ms_left;

    // Update the DOS/BIOS time in BDA?
}

//
// The master interrupt dispatcher and ISR of all interrupts.
// EAX and EDX pass the arguments for simplicity
//
__attribute__(( regparm(2), optimize("align-functions=64") ))
VOID InMasterDispatch(PDWORD regs, DWORD irq)
{
    WORD  inservice16 = InGetInService16();
    const DWORD v86_interrupted = regs[RD_EFLAGS] & (1<<17) != 0;

    /// 1. The ISR is set to zero for both PICs upon SpurInt.
    /// 2. If an spurious IRQ comes from master, no EOI is sent
    /// because there is no IRQ. if it is from the slave PIC
    /// EOI is sent to the master only

    last_mode = INTERRUPT;

    // Is this IRQ#0? If so, handle it directly
    if (BIT_IS_SET(inservice16, 0))
        HandleIRQ0(v86_interrupted, regs);

    // Is this a spurious IRQ? If so, do not handle.
    if (irq == 7 && (inservice16 & 0xFF) == 0)
        return;
    else if (irq == 15 && (inservice16 >> 8) == 0)
        return;

    if (InGetInterruptLevel(irq) == BUS_INUSE)
    {
        // Interrupts can fire during an ISR if there is nothing atomic going on
        IntsOn();
        InGetInterruptHandler(irq)(v86_interrupted, regs);
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
// This will generate a #GP when called
//
VOID Init_DPMI_ReflectionHandlers()
{
    BYTE entry = NON_SYSTEM_VECTORS;
    BYTE iteration_max_bnd = 256 - entry;

    for (entry=NON_SYSTEM_VECTORS; entry < iteration_max_bnd; entry++)
    {
        MkTrapGate(entry, 0, 0);
    }
}

//
// Called by #GP, everything is handled here
// If the kernel caused the error, we don't care. The main handler
// deals with that.
//
VOID CheckAndHandleDPMI_Trap(PDWORD regs)
{
    DWORD error_index   = ScGetFaultErrorCode();
    DWORD caused_by_idt = (error_index >> 1)&1;
    BYTE vector = (error_index >> 3) & 0x1FFF;

    if (caused_by_idt)
    {
        // Good, this matters to us now, upper bits of the selector error code
        // represent the index to the IDT entry
        // Now we must handle the virtual PM IDT

        switch (current_pcb->local_idt[vector].type)
        {
        case LOCAL_INT_PM_TRAP:
            // Before we destroy EIP and CS of the program, we must first
            // save it for when IRET is called by the program
            // We are emulating the whole instruction. Nothing is on the stack.
            // The solution is to push values to the stack manually.
            //
            // In case it comes up, the V86 implementation of IRET, while it
            // does not save anything on the stack, EnterV86 does keep track,
            // making it nestable.
            //

            // Set the address to return when handler is done
            regs[RD_EIP] = current_pcb->local_idt[vector].handler_address;
            regs[RD_CS]  = current_pcb->local_idt[vector].handler_code_segment;
        break;

        default:
        break;
        }
    }
}

//
// When the DPMI program executes IRET, we must go back to the cause of the interrupt
//
VOID HandleDPMI_IRET()
{}

//
// By default, the PIT is set to pitifully slow intervals, clocking at
// about 18.4 Hz (or IRQs per second). This is unsuitable
// for pre-emptive multitasking. We must configure this to a
// more satifactory frequency. I would like about 1000 Hz.
//
// The PIT has a base frequency of 1193800 Hz. We must set the division
// value to 1200 (0x4B0) to get an output frequency of 994.8 Hz.
//
static VOID ConfigurePIT()
{
    const BYTE count[2] = {0xB0, 0x4};

    outb(0x43, 0x36);
    outb(0x40, count[0]);
    outb(0x40, count[1]);
}

//
// The PnP manager must be initialized before the scheduler
// When the scheduler is enabled, INIT.EXE will automatically execute
// and interrupts will be enabled.
//
VOID InitScheduler()
{
    ConfigurePIT();
    Init_DPMI_ReflectionHandlers();

    // Claim the timer IRQ resource

    // Now get IRQ#13 and assign the ISR

    // The exception handlers are already installed by IA32.c

}
