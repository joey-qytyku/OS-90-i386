// Note: Variables shared by ISRs and kernel must be volatile because
// they can change unpredictably

#include <Resource.h>
#include <Type.h>
#include <IA32.h>
#include <V86.h>
#include <Scheduler.h>

// used by vm86.asm, automatically cleared
// by the gpf handler when it is set by VM86.asm
byte vm86_caused_gpf=0; // ?
dword current_proc=0;

static INTVAR byte  last_mode = KERNEL;
static INTVAR dword spurious_interrupts = 0;

// Only applicable to software interrupts
// Used when emulating INT XX?>

static unsigned long long uptime = 0; // Fixed point

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

static inline void SendEOI(byte vector)
{

    outb(0x20, 0x20);
    if (vector > 7)
        outb(0xA1, 0x20);
    IOWAIT();
}

/**
* Called from assembly, High level ISR for all IRQs
**/
void MiddleDispatch(PTrapFrame tf, dword irq)
{
    // Simpler way to do this? Use inlines for both ISRs?
    word inservice16 = GetInService16();
    PInterrupt intr  = GetIntInfo(irq);

    /* 1. The ISR is set to zero for both PICs upon SpINT.
     * 2. If an spurious IRQ comes from master, no EOI is sent
     * because there is no IRQ. if it is from the slave PIC
     * EOI is sent to the master only
    */

    last_mode = INTERRUPT;

    if (irq == 7 && (inservice16 & 0xFF) == 0)
    {
        goto CountAndRet; /* -_- */
    }
    else if (irq == 15 && (inservice16 >> 8) == 0)
    {
        SendEOI(0); // Send to master
        CountAndRet:
            spurious_interrupts++;
            return;
    }

    if (intr->intlevel == RECL_16)
    {
        // Disable interrupts
        // EOI will be sent by the ISR
    }
    else if (STANDARD_32 || TAKEN_32)
    {
        if (!intr->fast)
            IntsOn();

        intr->handler(tf);

        IntsOff();
        SendEOI(irq);
    }
}
/* SPURIOUS IRQ??? */

// Interrupt inting: what about the scheduler?
// runs with CLI?
// Time slice passed variable
// How can I make this faster?
int HandleIRQ0(PTrapFrame t)
{
    static bool time_slice_in_progress;
    static word ms_left;

    uptime += 0x10000000; // Trust me bro
}

void InitScheduler(void)
{
    int i;

    // Claim the timer IRQ resource
    // The handler is called by dispatcher directly for speed
    RequestFixedLines(1, NULL, &KERNEL_OWNER);

    /***
     * Each interrupt has its own vector so that the IRQ number
     * can be automatically deduced in the case of a spurious interrupt
    ***/
    // Index the ISRs since they are same size?

    /* The exception handlers */
}
