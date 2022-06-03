#include <Type.h>
#include <IA32.h>

void PIC_Remap(void)
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

byte PIC_ReadIMR() {}


void KernelMain(void *info)
{
    InitIA32();
    EarlyInitPIC();
    // Memset the allow and deny IOPBs
    // 1 indicates DENIED
}
