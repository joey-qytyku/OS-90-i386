#include <Platform/IA32.h>
#include <Platform/8259.h>
#include <Platform/X87.h>
#include <Intr_Trap.h>

#define IDT_SIZE 256

// Includes two IO bitmaps 
CompleteTSS main_tss;

static Gdesc gdt[GDT_ENTRIES] = {
    { /* Code ring 0 */
        .access = 0x9A,
        .base0  = 0,
        .base1  = 0, .base2  = 0,
        .limit  = 0xFFFF,
        .limit_gr = 0xCF
    },
    {   /* Data ring 0 */
        .access = 0x92,
        .base0  = 0,
        .base1  = 0, .base2  = 0,
        .limit  = 0xFFFF,
        .limit_gr = 0xCF
    },
    {
        /* Code ring 3 */
        .access = 0xF8,
        .base0  = 0,
        .base1  = 0, .base2  = 0,
        .limit  = 0xFFFF,
        .limit_gr = 0xCF
    },
    {   // Data ring 3
        .access = 0xF2, // Fix ???????
        .base0  = 0,
        .base1  = 0,
        .base2  = 0,
        .limit  = 0xFFFF,
        .limit_gr = 0x0F
    },
    {    // TSS descriptor
        .access = 0x89,
        .base0  = 0, // The kernel image is flat binary,
        .base1  = 0, // it is never relocated so putting
        .base2  = 0, // the address here should not work
        .limit  = sizeof(main_tss)-1,
        .limit_gr = 0
    }
};

// Table of exception vectors
static void (*except[EXCEPT_IMPLEMENTED])() =
{
    LowDivide0,
    LowDebug,
    LowNMI,
    LowBreakpoint,
    LowOverflow,
    LowBoundRangeExceeded,
    LowInvalidOp,
    LowDevNotAvail,
    LowDoubleFault,
    LowSegOverrun,
    LowInvalidTSS,
    LowSegNotPresent,
    LowStackSegFault,
    LowGeneralProtect,
    LowPageFault
};

static void (*irq_vectors[16])() =
{
    Low0,
    Low1,
    Low2,
    Low3,
    Low4,
    Low5,
    Low6,
    Low7,
    Low8,
    Low9,
    Low10,
    Low11,
    Low12,
    Low13,
    Low14,
    Low15
}

static Intd idt[IDT_SIZE];

// Ignore not used warnings, refferenced in StartK.asm
xDtr
gdtr = {.limit=sizeof(gdt)-1,.address=(dword)&gdt},
idtr = {.limit=0x2000,       .address=(dword)&idt};

// Will this work?
static inline void FillIDT(void)
{ // 15 exceptions, 16 IRQs? Think about it.
    int i, j;
    for (i = 0; i<EXCEPT_IMPLEMENTED &&  j<16; i++, j++)
    {
        MkTrapGate(idt, i, 0, except[i]);
        MkIntrGate(idt, EXCEPT_IMPLEMENTED+j, irq_vectors[j]);
    }
}

static void MkTaskSeg(CompleteTSS *tssptr)
{
    dword tss_int = (dword)tssptr;
    unsigned long i =
    GDT_RING0_TSS
        | ((tss_int << 16) & 0xFFFF)
        | ((tss_int << 32) & 0xFF)
        | ((0xC0)   << 56  & 0xFF);
}

/// @brief Reprogram the PICs
/// @section NOTES
/// Different bits tell OCWs and ICWs appart in CMD port
/// Industry standard architecture uses edge triggered interrupts
/// 8-byte interrupt vectors are default (ICW[2] = 0)
/// 8259.h contains info on IOWAIT and the IRQ_BASE
///
static void PIC_Remap(void)
{
    byte icw1 = ICW1 | ICW1_ICW4;

    // ICW1 to both PIC's
    outb(0x20, icw1);
    IOWAIT();
    outb(0xA0, icw1);
    IOWAIT();

    // ICW2, set interrupt vectors
    outb(0x21, IRQ_BASE);
    IOWAIT();
    outb(0xA1, IRQ_BASE+8);
    IOWAIT();

    // ICW3, set cascade
    outb(0x21, 4);  // ICW3, IRQ 2 is cascade (bitmap)
    IOWAIT();
    outb(0xA1, 2);  // ICW3 is different for slave PIC (index)

    outb(0x21, ICW4_8086);
    IOWAIT();
    outb(0xA1, ICW4_8086 | ICW4_SLAVE); // Assert PIC2 is slave
    IOWAIT();
}

/// @brief Detect the X87 and set it up if present
/// Read Resource.c for information on IRQ#13
///
/// @return
/// @retval 0 No FPU present
///
void SetupX87()
{
    // The Native Exceptions bit, when set, the FPU
    // exception is sent to the dedicated vector
    // otherwise, an IRQ is sent.
    // IRQ#13 is hardwired

    dword _cr0;
    __asm__ volatile ("mov %cr0, %0":"=r"(_cr0)::"memory");

    if (_cr0 & CR0) // Is there no FPU?
    {
        return 0;
    }

    // A 80287 could be used on 386 processors
    // The difference is minor

    // The NE bit is always zero on the 80386 and cannot be set
}

void InitIA32(void)
{
    MkTaskSeg(&main_tss);
    __asm__ volatile ("ltr (%0)"::"r"(GDT_TSSD<<3):"memory");

    C_memset(&main_tss.iopb_deny_all, '\xFF', 0x2000);

    PIC_Remap();

    //
    // Tell the PICs to send the ISR through CMD port reads
    // The kernel does need the IRR
    //
    outb(0x20,0xB);
    IOWAIT();
    outb(0xA0,0xB);
    IOWAIT();

    // Mask all interrupts, writes to data port is OCW3/IMR
    outb(0xA1,0xFF);
    IOWAIT();
    outb(0x21,0xFF);
    IOWAIT();

    /* Fill the IDT with the exception vectors */
    FillIDT();
}
