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

byte emulate_svi=0;
dword current_proc=0;

Mode last_mode = KERNEL;

void HandleGPF(dword error_selector)
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

// Technically not a far pointer, but reminds me of DOS C
inline pvoid MK_FP(word seg, word off)
{
    return (pvoid)((seg<<4) + off);
}

byte bPeek86(word seg, word off) {return *(pbyte)Segment(seg,off);}
word wPeek86(word seg, word off) {return *(pword)Segment(seg,off);}

void MonitorV86()
{
}

// Called from assembly, ISR for all IRQs
// Except for timer, which is written in ASM
void MiddleDispatch(PTrapFrame tf)
{
    byte v = GetIntVector();

    PInterrupt intr = GetIntInfo();

    switch (intr->intlevel)
    {
        case RECL_16:
            // Set IOPB in the TSS
            // Disable interrupts
            // EOI will be sent by the ISR
        break;
        case STANDARD_32: case TAKEN_32:
            if (intr->fast)
                __asm__ volatile ("cli");
            // Send EOI on behalf of callee
        default:;
    }
}

/* SPURIOUS IRQ??? */

void InitScheduler(void)
{
    int i;

    C_memset(&main_tss.iopb_deny_all, '\xFF', 0x2000);

    for (i=0; i<16;i++) {
        IA32_SetIntVector(IRQ_BASE+i, IDT_INT386, (pvoid)&MiddleDispatch);
    }
}

