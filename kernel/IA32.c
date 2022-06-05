#include <IA32.h>

CompleteTSS main_tss;

static Gdesc gdt[GDT_ENTRIES] = {
    { /* Code 32 */
        .access = 0x9A,
        .base0  = 0,
        .base1  = 0, .base2  = 0,
        .limit  = 0xFFFF,
        .limit_gr = 0xCF
    },
    {   /* Code extra (for PnP) */
        .access = 0x9A,
        .base0  = 0,
        .base1  = 0,
        .base2  = 0,
        .limit  = 0xFFFF,
        .limit_gr = 0x0F
    },
    {   /* Data 1 */
        .access = 0x92,
        .base0  = 0,
        .base1  = 0, .base2  = 0,
        .limit  = 0xFFFF,
        .limit_gr = 0xCF
    },
    {   /* Data 2 */
        .access = 0x92,
        .base0  = 0,
        .base1  = 0,
        .base2  = 0,
        .limit  = 0xFFFF,
        .limit_gr = 0x0F
    },
    {
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
void SetIntVector(byte v, byte attr, void *address)
{
    idt[v].attr = attr; // TODO
    idt[v].offset_15_0  = (dword)address &  0xFFFF;
    idt[v].offset_16_31 = (dword)address >> 16;
    idt[v].zero = 0;
}

static void PIC_Remap(void)
{
    byte icw1 = ICW1 | CASCADE | ICW1_ICW4 | LEVEL_TRIGGER;
    /* Notes: Different bits tell OCWs and ICWs appart in CMD port
     * Industry standard architecture uses edge triggered interrupts
     * 8-bytes interrupt vectors are default (ICW[2] = 0)
    */

    // ICW1 to both PIC's
    outb(0x20, icw1);
    outb(0xA0, icw1);

    // ICW2, set interrupt vectors
    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 4);  // ICW3, IRQ 2 is cascade (bitmap)
    outb(0xA1, 2);  // ICW3 is different for slave PIC (index)

    outb(0x21, ICW4_X86);
    outb(0xA1, ICW4_X86 | ICW4_SLAVE); // Assert PIC2 is slave
}

void InitIA32(void)
{
    dword _tss0 = (dword)(&main_tss);
    gdt[GDT_TSSD].base0 = (word)(_tss0 & 0xFFFF);
    gdt[GDT_TSSD].base1 = (byte)((_tss0 >> 16) & 0xFF);
    gdt[GDT_TSSD].base2 = 0xC0; // Its going to be this anyway

    __asm__ volatile ("ltr %0"::"r"(GDT_TSSD<<3));

    PIC_Remap();
}
