// Note: Variables shared by ISRs and kernel must be volatile because
// they can change unpredictably

// The FPU registers only need to be saved when
// another process tries to use them
// Scheduler priority for FPU switch?

#include <Platform/Resource.h>
#include <Platform/8259.h>
#include <Platform/IA32.h>
#include <Scheduler.h>
#include <Type.h>
#include <V86.h>

// used by vm86.asm, automatically cleared
// by the gpf handler when it is set by VM86.asm
byte vm86_caused_gpf=0; // ?
dword current_proc=0;

static INTVAR byte  last_mode = KERNEL;
static INTVAR dword spurious_interrupts = 0;

static unsigned long long uptime = 0; // Fixed point

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

void GeneralProtect()
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

    pic_outb(0x20, 0x20);
    if (vector > 7)
        pic_outb(0xA1, 0x20);
}

// Interrupt inting: what about the scheduler?
// runs with CLI?
// Time slice passed variable
// How can I make this faster?
static int HandleIRQ0(PTrapFrame t)
{
    static bool time_slice_in_progress;
    static word ms_left;

    uptime += 0x10000000; // Trust me bro
    return 0;
}

// EAX and EDX pass the arguments for simplicity
__attribute__(( regparam(2) ))
void InMasterDispatch(PTrapFrame tf, dword irq)
{
    // Simpler way to do this? Use inlines for both ISRs?
    word inservice16 = GetInService16();
    const PInterrupt intr  = FastGetIntInfo(irq);

    /// 1. The ISR is set to zero for both PICs upon SpINT.
    /// 2. If an spurious IRQ comes from master, no EOI is sent
    /// because there is no IRQ. if it is from the slave PIC
    /// EOI is sent to the master only

    last_mode = INTERRUPT;

    // Is this IRQ#0? If so, handle it directly
    if (BIT_IS_SET(inservice16, 0))
        HandleIRQ0(tf);

    // Is this a spurious IRQ? If so, do not handle.
    if (irq == 7 && (inservice16 & 0xFF) == 0)
        goto CountAndRet; // (-_-)
    else if (irq == 15 && (inservice16 >> 8) == 0)
    {
        SendEOI(0); // Send to master
        CountAndRet:
            spurious_interrupts++;
            return;
    }

    if (intr->intlevel == STANDARD_32 || intr->intlevel == TAKEN_32)
    {
        if (!intr->fast)
            IntsOn();

        intr->handler(tf); // Return value?

        IntsOff();
        SendEOI(irq);
    }
    else if (RECL_16)
    {
        // Disable interrupts
        // EOI will be sent by the ISR
    }
}

void InitScheduler()
{

    // Claim the timer IRQ resource
    // The handler is called by dispatcher directly for speed
    IntrRequestFixed(1, NULL, 1, KERNEL_OWNER);

    //
    // Each interrupt has its own vector so that the IRQ number
    // can be automatically deduced in the case of a spurious interrupt
    // Index the ISRs since they are same size?

    // The exception handlers are already installed by IA32.c

}
