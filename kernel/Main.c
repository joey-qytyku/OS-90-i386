#include <Type.h>
#include <x86.h>

static byte vm86_tss[103], main_tss[103];

static byte gdt = {
    0,0,0,0,0,0,0,0, // Null
    0xFF,0xFF,0,0,0,0x9A,0xCF,0, // Code
    0xFF,0xFF,0,0,0,0x92,0xCF,0  // Data
};

static Intd idt[256];

static xDtr
    gdtr = {.address=&gdt,.limit=sizeof(gdt)-1},
    idtr = {.address=&idt};


void SetIntVector(char v, byte attr)
{
    idt[v].attr = attr;
}

void EarlyInitPIC(byte map_to)
{
    byte icw1 = ICW1 | CASCADE | ICW1_ICW4 | LEVEL_TRIGGER;
    // Note: Different bits tell OCWs and ICWs appart in CMD port

    // ICW1 to both PIC's
    outb(icw1, 0x20);
    outb(icw1, 0xA0);

    // ICW2, set interrupt vectors
    outb(map_to     << 3, 0x21);
    outb((map_to+8) << 3, 0xA1);

    outb(4, 0x21);  // ICW3, IRQ_2 is cascade
    outb(2, 0xA1);  // ICW3 is different for slave PIC, cascade IRQ#

    outb(ICW4_X86, 0x21);
    outb(ICW4_X86 | ICW4_SLAVE, 0xA1); // Assert PIC2 is slave
}

void KernelMain(void *info)
{
    sbyte msg[] = "Hello, VGA world";

    for (int i=0;msg[i]!=0;*(char*)(0xB8000+i)=msg[i],i+=2);

//    __asm__ volatile ("lgdtl %0"::"rm"(gdtr));
//    __asm__ volatile ("lidtl %0"::"rm"(idtr));

    EarlyInitPIC(32);
}
