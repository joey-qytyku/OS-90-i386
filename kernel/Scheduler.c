/*
Everything process and control flow related
* Scheduler
* V86
* Interrupt handling
*/

#include <Scheduler.h>
#include <Resource.h>
#include <Type.h>
#include <IA32.h>

// used by vm86.asm, automatically cleared
// by the gpf handler when it is set by VM86.asm
byte vm86_caused_gpf=0
dword current_proc=0;

byte last_mode = KERNEL;
static dword spurious_interrupts = 0;

// Only applicable to software interrupts
// Used when emulating INT XX
void (*trap_capture[256]);

static unsigned long long uptime = 0;// Fixed point

void HandleGPF(dword error_selector) // Args correct?
{
    /*
     * The error code is always zero if it is not
     * segment related. There are no zero selectors
    */

   // Is it from the kernel?
   // If so PANIC

    // Is the GPF from VM86?

    // If not, terminate the current task
}


pvoid MK_LP(word seg, word off)
{   // Make linear pointer
    return (pvoid)((seg<<4) + off);
}

byte bPeek86(word seg, word off) {return *(pbyte)Segment(seg,off);}
word wPeek86(word seg, word off) {return *(pword)Segment(seg,off);}

void EmulateINT(pword stack, pdword ivt, PTrapFrame context)
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
void EmulateIRETW(pword stack, PTrapFrame context)
{
    context->regs.flags = *stack;
    context->regs.eip   =  stack[-1]+1;
    context->regs.cs    = (stack[-2] & 0xFFFF);
    context->regs.esp += 8;
}

void MonitorV86(PTrapFrame context)
{
    // After GPF, saved EIP points to the instruction, NOT AFTER
    pbyte ins   = MK_LP(context->cs, context->eip);
    pword stack = MK_LP(context->ss, context->esp);
    pdword ivt  = (pdword)phys(0);

    if (*ins == 0xCD) { /* INT (IMMED8) */
        // Has this interrupt been trapped?
        // Capturing required for disk access
        EmulateINT(stack, ivt, context);
        return;
    }
    else if (0xCF) { /* IRETW */
        // IRET can be called by any real mode software
        // and will not be given special meaning here
        // Return to CS:IP+1 in the stack, sizeof(iret) == 1
        EmulateIRETW(stack, context);
        return;
    }
}

inline void SendEOI(byte vector)
{
    if (vector > 7)
        outb(0xA1, 0x20);
    outb(0x20, 0x20)
    IOWAIT();
}

/**
* Called from assembly, High level ISR for all IRQs
**/
void MiddleDispatch(PTrapFrame tf, dword irq)
{
    word inservice16 = GetInService16();
    PInterrupt intr  = GetIntInfo(irq);
    // Ugh... a lot of if statements, can I do better?

    /* 1. The ISR is set to zero for both PICs upon SpINT.
     * 2. If an spurious IRQ comes from master, no EOI is sent
     * because there is no IRQ. if it is from the slave PIC
     * EOI is sent to the master only
    */

    if (irq == 7 && (inservice16 & 0xFF) == 0)
    CntAndRet:
        spurious_interrupts++;
        return;
    else if (irq == 15 && (inservice16 >> 8) == 0)
    {
        // EOI goes to the master if from the slave
        // Normally takes the IRQ, sends EOI to master
        SendEOI(0);
        goto CntAndRet; // -_-
    }

    if (intr->intlevel == RECL_16)
    {
        // Disable interrupts
        // EOI will be sent by the ISR
    }
    else if (STANDARD_32 || TAKEN_32)
        if (!intr->fast)
            IntsOn();

        intr->handler(tf);
        IntsOff();
        SendEOI(vector);
    }
}
/* SPURIOUS IRQ??? */

// Interrupt inting: what about the scheduler?
// runs with CLI?
// Time slice passed variable
// How can I make this faster?
void HandleIRQ0(PTrapFrame t)
{
    static bool time_slice_in_progress;
    static word ms_left;

    uptime += 0x10000000; // Trust me bro
}

void InitScheduler(void)
{
    int i;

    C_memset(&main_tss.iopb_deny_all, '\xFF', 0x2000);

    // Set the IDT entries to proper values
    for (i=0; i<16;i++)
        IA32_SetIntVector(IRQ_BASE+i, IDT_INT386, (pvoid)&MiddleDispatch);

    // Claim the timer IRQ resource
    // The handler is called by dispatcher directly for speed
    RequestFixedLines(1, NULL, &KERNEL_OWNER);

    /**
     * Each interrupt has its own vector so that the IRQ number
     * can be easily deduced in the case of a spurious interrupt
    ***/
}
