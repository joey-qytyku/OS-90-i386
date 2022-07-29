#if 0
V86 handling code

Timeline:
2022-07-21:
    All IRETs from IRQ or kernel DOS/BIOS calls are now exit codes
    If from user program, access violation causes temrination.
    This streamlines the V86 code a little.

#endif


#include <Platform/IA32.h>
#include <Platform/8259.h>
#include <Scheduler.h>
#include <Linker.h>     /* phys() */
#include <V86.h>        /* EnterV86, ShootdownV86 */
#include <Type.h>

static DRVMUT void (*trap_capture[128])(PTrapFrame*);
static INTVAR bool supervisor_call = 0;

const pdword real_mode_ivt = (pvoid)phys(0);

static inline pvoid MK_LP(word seg, word off)
{
    return (pvoid)((seg<<4) + off);
}

/* DOS will change the stack of an IRQ */

static byte bPeek86(word seg, word off) {return *(pbyte)MK_LP(seg,off);}
static word wPeek86(word seg, word off) {return *(pword)MK_LP(seg,off);}

//
// Brief:
//     Helper function that emulates the INT instruction for V86 mode
//     This is not for running BIOS calls.
// Arguments:
//  stack         A pointer the stack being used, pre-calculated by caller
//  context       A pointer to a trap frame, not from a saved context
//  v             Interrupt vector in the real mode IVT
//
static void EmulateINT(pword stack, PTrapFrame context, byte v)
{

    /**
     * Stack must be modified because a real mode ISR
     * may use the INT instruction
     * Note the direction of the IA-32 stack is reversed
    **/
    *stack    = (word)context->eflags; /* FLAGS */
    stack[-1] = (word)context->cs;     /* CS    */
    stack[-2] = (word)context->eip+2;  /* IP    */
    context->regs.esp -= 8; /* SP points to next value to use upon push */

    //
    // Continute execution at CS:IP in IVT
    //

    context->eip = (real_mode_ivt[v] & 0xFFFF);
    context->cs  =  real_mode_ivt[v] >> 16;
}


//
// Modifies the return context. This is so that
//
static void EmulateIRETW(pword stack, PTrapFrame context)
{
    // IRET can be called by any real mode software
    // and will not be given special meaning here
    // Return to CS:IP+1 in the stack, sizeof(iret) == 1

    // Keeps the top bits of EFLAGS, which should not be changed by real mode software
    context->eflags = *stack | (context->eflags & 0xFFFF0000);
    context->eip   =  stack[-1]+1;
    context->cs    = (stack[-2] & 0xFFFF);
    context->regs.esp += 8; // Correct?
}

// TODO:
//    Make IRET the stop code for all supervisor calls (INT and IRQ)
//    There is no reason why a user program would use IRET

// The V86 monitor for 16-bit tasks, ISRs, and PM BIOS/DOS calls
// Called by GP# handler
void ScMonitorV86(PTrapFrame context)
{
    // After GPF, saved EIP points to the instruction, NOT AFTER
    pbyte ins   = MK_LP(context->cs, context->eip);
    pword stack = MK_LP(context->ss, context->esp);

    //
    // Software interrupts may be called by anything and must be emulated
    // if that interrupt has not been captured by a VMM.
    //
    if (*ins == 0xCD)
    {
        // Has this interrupt been trapped?
        // If so, call the 32-bit trap handler instead
        // and pass it the context pointer
        if (trap_capture[ins[1]] != NULL)
            trap_capture[ins[1]](context);
        else EmulateINT(stack, context, ins[1]);
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
            // Summary:
            //      DOS program -> error, not supposed to use IRET
            //      IRQ         -> return to caller
            //      INTx        -> return to caller
            // Some programs may want to set an ISR, but the IRET
            // will never be reached by normal code.

            ScShootdownV86(); // Re-enter caller of EnterV86
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

