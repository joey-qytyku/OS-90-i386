#include <IA32.h>

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

void InitIA32(void)
{
    dword _tss0 = (dword)(&main_tss);
    gdt[TSS].base0 = (word)(_tss0 & 0xFFFF);
    gdt[TSS].base1 = (byte)((_tss0 >> 16) & 0xFF);
    gdt[TSS].base2 = 0xC0; // Its doing to be this anyway
}
