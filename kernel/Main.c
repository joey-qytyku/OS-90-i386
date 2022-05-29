#include <Type.h>
#include <IA32.h>

#define GDT_CODE32 0
#define GDT_CODE16 1
#define GDT_DATA1  2 // Data does not have a "size"
#define GDT_DATA2  3
#define GDT_TSS    4

// TODO: Make a struct
CompleteTSS main_tss;

static Gdesc gdt[] = {
    { /* Code 32 */
        .access = 0x9A,
        .base0  = 0,
        .base1  = 0, .base2  = 0,
        .limit  = 0xFFFF,
        .limit_gr = 0xCF
    },
    {   /* Code 16 */
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
    idtr = {.limit=0x2000, .address=&idt};



void SetIntVector(byte v, byte attr)
{
    idt[v].attr = attr;
}

void EarlyInitPIC(void)
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
    outb(0xA1, 2);  // ICW3 is different for slave PIC, (index instread)

    outb(0x21, ICW4_X86);
    outb(0xA1, ICW4_X86 | ICW4_SLAVE); // Assert PIC2 is slave
}

void KernelMain(void *info)
{
    dword _tss0 = (dword)(&main_tss);
    gdt[TSS].base0 = (word)(_tss0 & 0xFFFF);
    gdt[TSS].base1 = (byte)((_tss0 >> 16) & 0xFF);
    gdt[TSS].base2 = 0xC0; // Its doing to be this anyway

    EarlyInitPIC();

    // Memset the allow and deny IOPBs
    // 1 indicates DENIED
    C_memset(&main_tss.iopb_deny_all, '\xFF', 8192);
}
