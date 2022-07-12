#ifndef _8259_H
#define _8259_H

#include <Platform/IA32.h>
#include <Platform/IO.h>

#define ICW1 1<<4
#define LEVEL_TRIGGER 1<<3
#define ICW1_ICW4 1

#define ICW4_8086 1
#define ICW4_SLAVE 1<<3


/*
 * The last exception that I need to use is #XF because it
 * is supported by the Pentium III from 1999, only x64 CPUs
 * support VMX which is the next exception after that
 */
#define IRQ_BASE 0x20

/**
 * @brief Output with delay
 * Only present for i386 support. The 80486 and above
 * do not require the delay and it is kind of useless.
 * It is probably not required when interleaving IO, aka outputting to
 * different ports (faster than using the same ports).
 */
static inline void pic_outb(word port, byte val)
{
    outb(port, val);
    outb(0x80, 0); // Output to unused for
}

static inline byte pic_inb(word port)
{
    outb(0x80, 0);
    return inb(port);
}

//
// Memory clobber causes the statement to not be moved elsewhere to
// the compiler's convenience, useful for ensuring that memory access
// happens in the exact order specified rather than where GCC wants.
// "memory" can also be used for instructions that are not supposed to move
//
static inline void IntsOn (void) { __asm__ volatile ("sti":::"memory"); }
static inline void IntsOff(void) { __asm__ volatile ("cli":::"memory"); }

// The in-service register is a bit mask with one turned on
static inline word GetInService16(void)
{
    word in_service;

    in_service = pic_inb(0x20);
    in_service |= pic_inb(0xA0) << 8;
    return in_service;
}


#endif /* _8259_H */
