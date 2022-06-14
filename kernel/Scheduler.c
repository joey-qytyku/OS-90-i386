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

byte last_mode = KERNEL;

static unsigned long long uptime = 0;

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
    byte vector = InService();
    PInterrupt intr = GetIntInfo(vector);

    switch (intr->intlevel)
    {
        word port = 0x21;

        case RECL_16:
            // Set IOPB in the TSS
            // Disable interrupts
            // EOI will be sent by the ISR
        break;
        case STANDARD_32: case TAKEN_32:

            if (intr->fast)
                 IntsOff();
            else IntsOn();

            intr->handler();
            IntsOff();

            if (vector > 7)
                port = 0xA1;
            outb(port, 0x20);
            IOWAIT();
        default:;
    }
}
/* SPURIOUS IRQ??? */

// Interrupt inting: what about the scheduler?
// runs with CLI?
// Time slice passed variable
// How can I make this faster?
void HandleIRQ0(PTrapFrame t)
{
}

void InitScheduler(void)
{
    int i;

    C_memset(&main_tss.iopb_deny_all, '\xFF', 0x2000);

    // Set the IDT entries to proper values
    for (i=0; i<16;i++)
        IA32_SetIntVector(IRQ_BASE+i,IDT_INT386,(pvoid)&MiddleDispatch);

    // Claim the timer IRQ resource and assign a handler
    RequestIntLines(1, &HandleIRQ0, &KERNEL_OWNER);
}
