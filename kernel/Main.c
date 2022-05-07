#include <Type.h>
#include <PIC.h>
#include <IO.h>

#define INT386  0xE
#define TRAP386 0xF

typedef struct __attribute__((packed))
{
    word    offset_15_0;
    word    selector;
    byte    zero;
    byte    attr;
    word    offset_16_31;
    // Trap gates use the DPL
    // Interrupt gates do not
}Intd;

byte vm86_tss[103], main_tss[103];
Intd int_desc_tab[256];

struct xDtr {
    dword   address;
    word    limit;
}__attribute__((packed));

static struct xDtr
    gdtr = {.address=,.limit=},
    idtr = {.address=};

static inline void outb(short port, char val)
{
    __asm__ volatile ("outb %0, %1":: "a"(val), "Nd"(port));
}

void SetIntVector(char v, byte attr)
{
    int_desc_tab[v].attr = attr;
}

void EarlyInitPIC(byte map_to)
{
    byte icw1 = ICW1 | CASCADE | ICW1_ICW4 | LEVEL_TRIGGER;
    // Note: Different bits tell OCWs and ICWs appart in CMD port

    // ICW1 to both PIC's
    outb(icw1, 0x20);
    outb(icw1, 0xA0);

    // ICW2, set interrupt vectors
    outb( map_to    << 3,   0x21);
    outb((map_to+8) << 3,   0xA1);

    outb(4, 0x21);  // ICW3, IRQ_2 is cascade
    outb(2, 0xA1);  // ICW3 is different for slave PIC, cascade IRQ#

    outb(ICW4_X86,              0x21);
    outb(ICW4_X86 | ICW4_SLAVE, 0xA1); // Assert PIC2 is slave
}

void KernelMain(void)
{
    int cpuid_found;
    sbyte msg[] = "Hello, VGA world";

    for (int i=0;msg[i]!=0;*(char*)(0xB8000+i)=msg[i],i+=2);

    EarlyInitPIC(32);

    // Is this a i486/supports CPUID?
    __asm__ volatile (
        "pushfd\n"
        "pop %eax\n"
        "test $0x200000, %eax\n"
        "mov $0,%eax\n" // Does not chaneg flags
        "adc $0,%eax"
        :"a"(cpuid_found)::"eax"
    );

    if (cpuid_found) {
        // Enable native FPU exceptions on i486+
        asm volatile (
            "mov    %cr0,%eax \n"
            "or     $,%eax"
        );
    } else {
        // This is an i386, does it have an FPU
        // use coprocessor segment overrun and FPU IRQ13
        // No harm done if there is no x87
    }

}
