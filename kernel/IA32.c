#include <IA32.h>

// Includes two IO bitmaps 
CompleteTSS main_tss;

static Gdesc gdt[GDT_ENTRIES] = {
    { /* Code kernel */
        .access = 0x9A,
        .base0  = 0,
        .base1  = 0, .base2  = 0,
        .limit  = 0xFFFF,
        .limit_gr = 0xCF
    },
    {   /* Data kernel */
        .access = 0x92,
        .base0  = 0,
        .base1  = 0, .base2  = 0,
        .limit  = 0xFFFF,
        .limit_gr = 0xCF
    },
    {
        /* Code ring 3 */
        .access = 0xF8
        .base0  = 0,
        .base1  = 0, .base2  = 0,
        .limit  = 0xFFFF,
        .limit_gr = 0xCF
    },
    {   /* Data ring 3*/
        .access = 0xF2, // Fix
        .base0  = 0,
        .base1  = 0,
        .base2  = 0,
        .limit  = 0xFFFF,
        .limit_gr = 0x0F
    },
    {    /* TSS descriptor */
        .access = 0x89,
        .base0  = 0,
        .base1  = 0,
        .base2  = 0,
        .limit  = sizeof (main_tss)-1,
        .limit_gr = 0
    },
};

static Intd idt[256];

static xDtr
    gdtr = {.limit=sizeof(gdt)-1, .address=&gdt},
    idtr = {.limit=0x2000,        .address=&idt
};

// Low-level function
void IA32_SetIntVector(byte v, byte attr, pvoid address)
{
    idt[v].attr = attr; // TODO
    idt[v].offset_15_0  = (dword)address &  0xFFFF;
    idt[v].offset_16_31 = (dword)address >> 16;
    idt[v].zero = 0;
}

void MkTrapGate(byte vector, pvoid address)
{
    IA32_SetIntVector(vector, IDT_TRAP386, address);
}

void MkIntrGate(byte vector, pvoid address)
{
    IA32_SetIntVector(vector, IDT_INT386, address);
}

static void PIC_Remap(void)
{
    byte icw1 = ICW1 | CASCADE | ICW1_ICW4 | LEVEL_TRIGGER;
    /* Notes: Different bits tell OCWs and ICWs appart in CMD port
     * Industry standard architecture uses edge triggered interrupts
     * 8-byte interrupt vectors are default (ICW[2] = 0)
     * And required for the
    */

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

    outb(0x21, ICW4_X86);
    IOWAIT();
    outb(0xA1, ICW4_X86 | ICW4_SLAVE); // Assert PIC2 is slave
    IOWAIT();
}

// The in-service register is a bit mask with one turned on
word GetInService16(void)
{
    word in_service;

    IOWAIT();
    in_service = inb(0x20);
    IOWAIT();
    in_service |= inb(0xA0) << 8;
    IOWAIT();
    return in_service;
}

void InitIA32(void)
{
    dword _tss0 = (dword)(&main_tss);
    // I may want to find a better way to do this
    gdt[GDT_TSSD].base0 = (word)(_tss0 & 0xFFFF);
    gdt[GDT_TSSD].base1 = (byte)((_tss0 >> 16) & 0xFF);
    gdt[GDT_TSSD].base2 = 0xC0; // Its going to be this anyway

    // The TSS descriptor cache needs to be set
    // once the TSS entry has been properly initialized
    __asm__ volatile ("ltr %0"::"r"(GDT_TSSD<<3));

    // 1 in the IOPB means DENY
    C_memset(&main_tss.iopb_deny_all, '\xFF', 0x2000);

    // The PICs are ready to send the ISR from CMD port +0 
    // The IRR is not used 
    PIC_Remap();
    outb(0x20,0xB);
    IOWAIT();
    outb(0xA0,0xB);
    IOWAIT();

    // Mask all interrupts, individualy enabled
    outb(0xA1,0xFF);
    IOWAIT();
    outb(0x20,0xFF);
    IOWAIT();
}
