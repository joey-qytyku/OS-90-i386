static DRVMUT void (*trap_capture[256]);

/* monitor_iret32:
 *  An IRET in 16-bit code terminates the ISR and traps to kernel
 *  instead of continuing execution in V86M.
 * Modified by:
 *  Irq16
*/
static bool monitor_iret32 = 0;

inline pvoid MK_LP(word seg, word off)
{   // Make linear pointer
    return (pvoid)((seg<<4) + off);
}

static byte bPeek86(word seg, word off) {return *(pbyte)Segment(seg,off);}
static word wPeek86(word seg, word off) {return *(pword)Segment(seg,off);}

static void EmulateINT(pword stack, pdword ivt, PTrapFrame context)
{
    // Stack must be modified because a real mode ISR
    // may use the INT instruction
    // Note the direction of the IA-32 stack is reversed
    *stack    = (word)context->eflags; /* FLAGS */
    stack[-1] = (word)context->cs;     /* CS    */
    stack[-2] = (word)context->eip+2;  /* IP    */
    context->regs.esp -= 8; /* SP points to next value to use upon push */

    // Continute execution at CS:IP in IVT
    // IP+2 is after the INT
    context->regs.eip = (ivt[ins[1]] & 0xFFFF) + 2;
    context->regs.cs  = ivt[ins[1]] >> 16;
}

/* CHANGES RETURN CONTEXT */
static void EmulateIRETW(pword stack, PTrapFrame context)
{
    // IRET can be called by any real mode software
    // and will not be given special meaning here
    // Return to CS:IP+1 in the stack, sizeof(iret) == 1
    context->regs.flags = *stack;
    context->regs.eip   =  stack[-1]+1;
    context->regs.cs    = (stack[-2] & 0xFFFF);
    context->regs.esp += 8;
}

void Irq16(byte vector) {
    static pdword ivt = phys(0);

    static TrapFrame tf = {
        .cs =  ivt[vector] >> 16,
        .eip = ivt[vector] &  0xFFFF,
        .eflags = 1<<17
    };
    monitor_iret32 = true;
    EnterV86(tf);
}

static void MonitorV86(PTrapFrame context)
{
    // After GPF, saved EIP points to the instruction, NOT AFTER
    pbyte ins   = MK_LP(context->cs, context->eip);
    pword stack = MK_LP(context->ss, context->esp);
    pdword ivt  = (pdword)phys(0);

    if (*ins == 0xCD) { /* INT (IMMED8) */
        // Has this interrupt been trapped?
        // Capturing required for disk access
        EmulateINT(stack, ivt, context);
    }
    else if (0xCF) /* IRETW */
    {
        /* When an IRQ happens, the real mode stack
         * will be updated but execution continues in protected mode
         * using the PM stack. This means that the context must not change
         * IRET is assumed to be used ONLY for exiting out of the interrupt handler
         * There is simply no other way to differentiate virtual IRETs. 16 traps require
         * emulation because 16-bit execution continues for the rest of the 16-bit program.
        */
        if (monitor_iret32)
        {
            /* Applicable to interrupt handlers:
             * An IRQ is sent to real mode using EnterV86(), called by Irq16()
             * The trap frame passed to it will be modified when the V86 code is
             * executing. Because IRET terminates the ISR to go to kernel
             * it return to the caller of EnterV86.
            ***/
            ShootdownV86(); // Re-enter caller of EnterV86
        } else {
            EmulateIRETW(stack, context);
        }
    }
    else if (0xFA) IntsOff(); /* CLI */
    else if (0xFB) IntsOn();  /* STI */
    else {
        // Error, if this is a supervisor call, critical error happened
    }
}

