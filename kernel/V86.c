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
#include <V86.h>        /* EnterV86, ShootdownV86 */
#include <Type.h>

#define CAPTURE_DOS_FUNCTIONS 256

// Does the capture chain list need to be volatile?
// Software will modify it externally, but does this matter if
// it does so with library functions?
//

//
// An array of stub handlers. They will simply return CAPT_NOHND
// so that DOS gets it. When new links are added, they will point to
// the next link in the chain. Index to this array is the vector.
//
V86_CHAIN_LINK v86_capture_chain[CAPTURE_DOS_FUNCTIONS];

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

const PDWORD real_mode_ivt = (PVOID)0;

static inline PVOID MK_LP(WORD seg, WORD off)
{
    DWORD address = seg*16 + off;
    return (PVOID) address;
}

static BYTE bPeek86(WORD seg, WORD off) {return *(PBYTE)MK_LP(seg,off);}
static WORD wPeek86(WORD seg, WORD off) {return *(PWORD)MK_LP(seg,off);}

//
// Brief: Insert a new link to a chain. Simple process.
// No return value.
//
APICALL VOID ScHookDosTrap(
                      VINT                      vector,
                      OUT PV86_CHAIN_LINK       new,
                      IN  V86_HANDLER           hnd
){
     const PV86_CHAIN_LINK prev_link = &v86_capture_chain[vector];

     prev_link->next = new;
     new->handler = hnd;
     new->next = NULL;
}

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
// CR3 is not modified at all so if caller memory must be used by the code
// then this is not enough.
//
// When the task just switched out of is 16-bit, CR3 must be modified
// and then restored to go back.
//
// context:
//      The register params or the state of the 16-bit program
//      Memory mapping must be set up separately, as stated above
// context stack: Automatically set
// for supervisor calls
//
VOID ScVirtual86_Int(IN PDWORD context, BYTE vector)
{
    PV86_CHAIN_LINK current_link;

    supervisor_call = 1;

    // A null handler is an invalid entry
    // Iterate through links, call the handler, if response is
    // CAPT_NOHND, call next handler.
/*
    current_link = &v86_capture_chain[vector];

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
*/

    // Fall back to real mode, in this case, the trap is of no
    // interest to any 32-bit drivers, and must go to the real mode
    // IVT. During this process, the INT instruction

    // Changing the context is not enough, I actually need to
    // go to real mode using ScEnterV86, and I have to duplicate
    // the passed context because I should not be changing CS and EIP
    DWORD new_context[RD_NUM_DWORDS];
    C_memcpy(new_context, context, RD_NUM_DWORDS*4);

    new_context[RD_CS]  = real_mode_ivt[vector] >> 16;
    new_context[RD_EIP] = real_mode_ivt[vector] & 0xFFFF;

    IaIOPB_Allow();

    // ScEnterV86 is thread-safe because it uses the stack to store
    // register states instead of global variables.
    // -> When this function is called, ScVirtual_Int will only continue
    // when ShootdownV86 is called.
    ScEnterV86(new_context);

    IaIOPB_Deny();
}

// The V86 monitor for 16-bit tasks, ISRs, and PM BIOS/DOS calls
// Called by GP# handler
VOID ScMonitorV86(IN PDWORD context)
{
    // After GPF, saved EIP points to the instruction, NOT AFTER
    PBYTE ins   = MK_LP(context[RD_CS], context[RD_EIP]);
    PWORD stack = MK_LP(context[RD_SS], context[RD_ESP]);

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

    // The following is for emulating privileged instructions
    // These may be used by an ISR or by the
    // kernel to access DOS/BIOS INT calls.

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

            // Re-enter caller of ScEnterV86, this will not enter
            // immediately, only after the #GP handler returns.
            // Reflecting interrupts also uses EnterV86
            ScOnExceptRetReenterCallerV86();
            return;
        break;

        case 0xFA: /* CLI */
            IntsOff();
            context[RD_EIP]++;
        break;
        case 0xFB: /* STI */
            IntsOn();
            context[RD_EIP]++;
        break;

        case 0xCE: /* INTO */
        break;

        // INT3 and INTO?

        default:
            // Nuke this process
        break;
        }// END OF SWITCH
    }
    if (!supervisor_call)
    {
    }

    // On return, code continues to execute or re-enters caller
    // of EnterV86
}

STATUS V86CaptStub()
{
     return CAPT_NOHND;
}

//
// Must be called before using anything in this file.
// The scheduler does NOT need to be initialized for V86
//
VOID InitV86(VOID)
{
     for (WORD i = 0; i<CAPTURE_DOS_FUNCTIONS; i++)
     {
        V86_CHAIN_LINK new = {
               .next = NULL,
               .handler = V86CaptStub
        };
        v86_capture_chain[i] = new;
     }
}
