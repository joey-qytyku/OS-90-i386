#ifndef X86_H
#define X86_H

#include <Type.h>

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

typedef struct __attribute__((packed))
{
    dword   address;
    word    limit;
}xDtr;

/********** 8259A DEFINES **********/

#define ICW1 1<<4
#define LEVEL_TRIGGER 1<<3
#define CASCADE 2
#define ICW1_ICW4 1

#define ICW4_X86 1
#define ICW4_SLAVE 1<<3

/********* PORT IO DEFINES *********/

static inline byte inb(short port)
{
byte ret;
__asm__ volatile ("inb %1, %0":"=a"(ret) : "Nd"(port));
return ret;
}

static inline byte outb(short port, byte val)
{
byte ret;
__asm__ volatile ("outb %0, %1"::
    "a"(val),
    "Nd"(port));
return ret;
}


static inline void rep_insb(void *mem, dword count, word port)
{
__asm__ volatile ("rep insb"::"esi"(mem),"ecx"(count),"dx"(port));
}

static inline void rep_outsb(void *mem, dword count, word port)
{
__asm__ volatile ("rep outsb"::"esi"(mem),"ecx"(count),"dx"(port));
}

static inline void rep_insw(void *mem, dword count, word port)
{
__asm__ volatile ("rep insw"::"esi"(mem),"ecx"(count),"dx"(port));
}

static inline void rep_outsw(void *mem, dword count, word port)
{
__asm__ volatile ("rep outsw"::"esi"(mem),"ecx"(count),"dx"(port));
}

#endif
