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
//     This is not for running BIOS calls. executing_an_isr EXPECTED ZERO
// Arguments:
//  stack         A pointer the stack being used, pre-calculated by caller
//  context       A pointer to the trap frame
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

/* CHANGES RETURN CONTEXT */
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
    // kernel to access DOS/BIOS INT calls
    //

    switch (*ins)
    {
    case 0xCF: /* IRETW */
        /* When an IRQ happens, the real mode stack will be updated but
         * execution continues in protected mode using the PM stack.
         * This means that the context must not change. IRET is assumed
         * to be used ONLY for exiting out of the interrupt handler
         * There is simply no other way to differentiate virtual IRETs. 16-bit traps require
         * emulation because 16-bit execution continues for the rest of the 16-bit program.
        */
        if (executing_an_isr)
        {
            /* Applicable to interrupt handlers:
             * An IRQ is sent to real mode using EnterV86(), called by Irq16()
             * The trap frame passed to it will be modified when the V86 code is
             * executing. Because IRET terminates the ISR to go to kernel
             * it returns to the caller of EnterV86.
            ***/
            ScShootdownV86(); // Re-enter caller of EnterV86
        } else {
            // In this case, IRETW was used by a 
            EmulateIRETW(stack, context);
        }
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
        // Error, if this is a supervisor call, critical error happened
        // Segment changing instructions do not require emulation
    break;
    }
}

