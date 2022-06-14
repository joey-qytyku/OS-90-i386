#ifndef X86_H
#define X86_H

#include <Type.h>

#define IDT_INT386  0xE /* Interrupts from hardware */
#define IDT_TRAP386 0xF /* Software INT calls, technically not interrupts but traps */

#define IRQ_BASE 0x20 /* Both PICs are mapped starting here */

enum {
GDT_KCODE,
GDT_KDATA,
GDT_UCODE,
GDT_UDATA,
GDT_TSSD,
GDT_ENTRIES
};

typedef dword Page;

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
    word    limit, base0;
    byte    base1;
    byte    access;
    byte    limit_gr;
    byte    base2;
}Gdesc;

// The standard register dump
// ESP is nonsense
typedef struct {
    dword   eax, ebx, ecx, edx, esi, edi, ebp, esp;
}RegsIA32,*PRegsIA32;

// If kernel to kernel switch, ss and esp are invalid
typedef struct __PACKED {
/**
 * When a task switch takes place, the CPU
 * pushes these values on the ESP0 stack
 * ESP+48 is the start of the trap frame
**/
    dword      ss,esp,eflags,cs,eip;
    RegsIA32   regs; // The lower-half handler saves these
}TrapFrame,*PTrapFrame;

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
    dword   link; // Zero extended
    dword
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

/********* PORT IO DEFINES *********/
// Check these?

static inline void outb(word port, byte val)
{
    asm volatile ("outb %0, %1": :"a"(val), "Nd"(port));
}

static inline void IOWAIT() {outb(0x80,0);}

static inline byte inb(word port)
{
    byte ret;
    asm volatile ("inb %1, %0" :"=a"(ret) :"Nd"(port) );
    return ret;
}

static IntsOn()  { __asm__ volatile ("sti"); }
static IntsOff() { __asm__ volatile ("cli"); }

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

#ifndef __PROGRAM_IS__DRIVER

extern void InitIA32(void);
extern void IA32_SetIntVector(byte, byte, pvoid);
extern byte InService();
extern struct CompleteTSS main_tss

#endif /* __PROGRAM_IS__DRIVER */

#endif
