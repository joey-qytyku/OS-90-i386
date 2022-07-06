#ifndef _8259_H
#define _8259_H

#include <Platform/IA32.h>

#define ICW1 1<<4
#define LEVEL_TRIGGER 1<<3
#define ICW1_ICW4 1

#define ICW4_8086 1
#define ICW4_SLAVE 1<<3


/*
 * The last exception that I need to use is #XF because it
 * is supported by the Pentium III from 1999, only x64 CPUs
 * support VMX which is the next exception
 */
#define IRQ_BASE 0x20

/**
 * @brief Delay IO
 * Only present for i386 support. The 80486 and above
 * do not require this function and it is kind of useless.
 * It is not required when interleaving IO, aka outputting to
 * different ports (faster than using the same ports).
 */
static inline void IOWAIT(void) {outb(0x80,0);}

/*
 * Memory clobber causes the statement to not be moved elsewhere to
 * the compiler's convenience, useful for ensuring that memory access
 * happens in the exact order specified rather than where GCC wants.
 * "memory" can also be used for instructions that are not supposed to move
 */

static inline void IntsOn (void) { __asm__ volatile ("sti":::"memory"); }
static inline void IntsOff(void) { __asm__ volatile ("cli":::"memory"); }

// The in-service register is a bit mask with one turned on
static inline word GetInService16(void)
{
    word in_service;

    IOWAIT();
    in_service = inb(0x20);
    IOWAIT();
    in_service |= inb(0xA0) << 8;
    IOWAIT();
    return in_service;
}


#endif /* _8259_H */
