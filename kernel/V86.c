
/*
     This file is part of OS/90.

    OS/90 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

    OS/90 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>. 


V86 handling code

Timeline:
2022-07-21:
    All IRETs from IRQ or kernel DOS/BIOS calls are now exit codes
    If from user program, access violation causes termination.
    This streamlines the V86 code a little.
2022-08-01
    Handling of TSS: Do I have to update it? Yes.
2022-08-06
    Remove pointer-to-stack parameter in EmulateInt

*/

#include <Platform/IA32.h>
#include <Platform/8259.h>
#include <Scheduler.h>
#include <Linker.h>     /* phys() */
#include <V86.h>        /* EnterV86, ShootdownV86 */
#include <Type.h>

#define CAPTURE_DOS_FUNCTIONS 256

MCHUNX v86_capture_chain[CAPTURE_DOS_FUNCTIONS];

// TSS and V86 mode:
// EnterV86 saves ESP first in TSS.ESP0.
// This is because the caller of ScVirtual86_Int
// is supposed to be fully re-entrant. If a GPF
// takes place, the fault must be handled using
// the same stack the kernel was already using
//
// Because task switching involves changing the
// value of TSS.ESP0, task switching cannot happen
// while in V86 mode or a different 32-bit
// kernel stack will be used and break everything
//
// Wait: what about 16-bit tasks running in V86?

//
// Determines if the supervisor is emulating a user program
// or a 16-bit interrupt/kernel call
static INTVAR BOOL supervisor_call = 0;

const PDWORD real_mode_ivt = (PVOID)phys(0);

static inline PVOID MK_LP(WORD seg, WORD off)
{
    DWORD address = seg*16 + off;
    return (PVOID) address;
}

static BYTE bPeek86(WORD seg, WORD off) {return *(PBYTE)MK_LP(seg,off);}
static WORD wPeek86(WORD seg, WORD off) {return *(PWORD)MK_LP(seg,off);}

//
//
//
ScAppendTrapLink()
{}

// Brief: Upon a critical error, it is necessary to
// remove all V86 links so that a bluescreen can be generated
// Modifies: v86_capture_chain
VOID ScOnErrorDetatchLinks(VOID)
{
    C_memset(&v86_capture_chain, '\0', sizeof(v86_capture_chain));
}

// Brief: General purpose BIOS/DOS call interface
// this function goes through capture and should be
// used by drivers and the kernel to access
// INT calls from protected mode.
//
// context:
//      The register params or the state of the 16-bit program
//      Memory mapping must be set up separately
// context stack: Automatically set
// for supervisor calls
//
VOID ScVirtual86_Int(IN PTRAP_FRAME context, BYTE vector)
{
    PV86_CHAIN_LINK current_link;

    // A null handler is an invalid entry
    // Iterate through links, call the handler, if response is
    // CAPT_NOHND, call next handler.

    current_link = v86_capture_chain[vector];

    // As long as there is another link
    while (current_link->next != NULL)
    {
        STATUS hndstat = current_link->handler(context);
        if (hndstat == CAPT_HND)
            return;
        else {
            current_link = current_link->next;
            continue;
        }
    }

    // Fall back to real mode, in this case, the trap is of no
    // interest to any 32-bit drivers, and must go to the real mode
    // IVT. During this process, the INT instruction

    // Changing the context is not enough, I actually need to
    // go to real mode using ScEnterV86, and I have to duplicate
    // the passed context because I should not be changing CS and EIP
    TRAP_FRAME new_context = *context;

    new_context.cs  = real_mode_ivt[vector] >> 16;
    new_context.eip = (WORD)real_mode_ivt[vector];

    IaIOPB_Allow();

    // ScEnterV86 is thread-safe because it uses the stack to store
    // register states instead of global variables.
    // -> When this function is called, ScVirtual_Int will only continue
    // when ShootdownV86 is called.
    ScEnterV86(&new_context);

    IaIOPB_Deny();
}

// The V86 monitor for 16-bit tasks, ISRs, and PM BIOS/DOS calls
// Called by GP# handler
void ScMonitorV86(IN PTRAP_FRAME context)
{
    // After GPF, saved EIP points to the instruction, NOT AFTER
    PBYTE ins   = MK_LP(context->cs, context->eip);
    PWORD stack = MK_LP(context->ss, context->esp);

    // Software interrupts may be called by anything and must be emulated
    // if that interrupt has not been captured by a driver
    // Interrupt service routines may also call interrupts. Such nesting is
    // possible because ScVirtual86_Int and EnterV86 are thread safe
    // If the INT instruction is found in an ISR, all that has to happen is a
    // change in program flow, with IRET instead representing a return to the
    // 16-bit caller ISR.

    if (*ins == 0xCD)
    {
        ScVirtual86_Int(context, ins[1]);
        return; // Nothing else to do now
    }

    //
    // The following is for emulating privileged instructions
    // These may be used by an ISR or by the
    // kernel to access DOS/BIOS INT calls.
    //
    if (supervisor_call)
    {
        switch (*ins)
        {
        case 0xCF: /* IRETW */
            // IRET is treated as the termination of both an ISR and a software
            // interrupt by the V86 monitor. This is because IRET has no purpose
            // besides that and in the case of IRQs, there is not another
            // point to terminate the ISR and return to the kernel.
            //
            // Sometimes, interrupt service routines use the INT instruction
            // (DOS uses the BIOS for most things), which requires different
            // treatment of the INT and IRET instructions.
            // Summary:
            //      DOS program -> error, not supposed to use IRET
            //      IRQ         -> return to caller
            //      INTx        -> return to caller
            // Some programs may want to set an ISR, but the IRET
            // will never be reached by normal code



            ScShootdownV86(); // Re-enter caller of ScEnterV86
        break;

        case 0xFA: /* CLI */
            IntsOff();
            context->eip++;
        break;
        case 0xFB: /* STI */
            IntsOn();
            context->eip++;
        break;

        default:
            // CRITICAL ERROR, btw mov sreg,r16 needs no emulation
        break;
        }// END OF SWITCH
    }

} // On return, code continues to execute

