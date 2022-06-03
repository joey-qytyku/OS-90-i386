#ifndef X86_H
#define X86_H

#include <Type.h>

#define INT386  0xE
#define TRAP386 0xF

#define __PACKED __attribute__((packed))

typedef dword page;

/* Data structures */

typedef struct __PACKED
{
    word    offset_15_0;
    word    selector;
    byte    zero;
    byte    attr;
    word    offset_16_31;
    // Trap gates use the DPL
    // Interrupt gates do not
}Intd;

typedef struct __PACKED
{
    word limit, base0;
    byte base1;
    byte access;
    byte limit_gr;
    byte base2;
}Gdesc;

typedef struct {
    dword   eax,ebx,ecx,edx,ebp,esp,esi,edi;
}Context,*PContext;

// If kernel to kernel switch, ss and esp are invalid
typedef struct __PACKED {
    dword      ss,esp,eflags,cs,eip;
    Context    regs;
}State,*PState;

typedef struct __attribute__((packed))
{
    word    limit;
    dword   address;
}xDtr;

typedef struct __attribute__((packed))
{
    pvoid   off;
    word    seg;
}FarPointer32;

typedef struct {
    dword   link;
    word
        esp0, ss0,
        esp1, ss1,
        esp2, ss2;
    dword   esp2, cr3, eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    dword   es, cs, ss, ds, fs, gs, ldtr;
    word    iobp_off;
    byte    iopb_allow_all[8192];
    byte    iopb_deny_all [8192];
    // I don't have to memset the IOPB now
}CompleteTSS;

/********** 8259A DEFINES **********/

#define ICW1 1<<4
#define LEVEL_TRIGGER 1<<3
#define CASCADE 2
#define ICW1_ICW4 1

#define ICW4_X86 1
#define ICW4_SLAVE 1<<3

extern void InitIA32(void);

/********* PORT IO DEFINES *********/
// Check these?

static inline void outb(word port, byte val)
{
    asm volatile ("outb %0, %1": :"a"(val), "Nd"(port));
}

static inline byte inb(word port)
{
    byte ret;
    asm volatile ("inb %1, %0" :"=a"(ret) :"Nd"(port) );
    return ret;
}

static inline void rep_insb(pvoid mem, dword count, word port)
{__asm__ volatile ("rep insb"::"esi"(mem),"ecx"(count),"dx"(port) :"esi","edi","dx");
}

static inline void rep_outsb(pvoid mem, dword count, word port)
{__asm__ volatile ("rep outsb"::"esi"(mem),"ecx"(count),"dx"(port) :"esi","edi","dx");
}

static inline void rep_insw(pvoid mem, dword count, word port)
{__asm__ volatile ("rep insw"::"esi"(mem),"ecx"(count),"dx"(port) :"esi","edi","dx");
}

static inline void rep_outsw(pvoid mem, dword count, word port)
{__asm__ volatile ("rep outsw"::"esi"(mem),"ecx"(count),"dx"(port) :"esi","edi","dx");
}

/* Other instructions */

void invlpg(page addr)
{// Linux defines like this
    __asm__ volatile ("invlpg (%0)"::"r"(addr) :"memory");
}

#endif
