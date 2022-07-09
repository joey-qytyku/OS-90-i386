#ifndef X86_H
#define X86_H

#include <Type.h>

/**
 * The only difference between trap and interrupt gates
 * is that interrupt gates clear interrupts upon entry
 * and trap gates do not. The EIP points to the address of
 * the next intruction for software and external hardware sources.
 * Internal CPU faults, however, save the address of the failed instruction
 * so that it may be restarted (e.g. page fault) once the error is corrected.
 */

#define IDT_INT386  0xE /* IF=0 on entry */
#define IDT_TRAP386 0xF /* IF=1 */

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

#define GDTDEF_R0_CSEG 0x00cf9a000000ffffULL
#define GDTDEF_R0_DSEG 0x00cf92000000ffffULL
#define GDTDEF_R3_CSEG 0x00cff8000000ffffULL
#define GDTDEF_R3_DSEG 0x000ff2000000ffffULL
#define GDTDEF_R0_TSS  0x0000890000002068ULL

// The standard register dump
// ESP is nonsense
typedef struct {
    dword   eax, ebx, ecx, edx, esi, edi, ebp, esp;
}RegsIA32,*PRegsIA32;

/**
 * When a task switch takes place, the CPU
 * pushes these values on the ESP0 stack
 * ESP+48 is the start of the trap frame
**/
typedef struct __PACKED {

    dword      eip,cs,eflags,ss,esp;
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
    dword   cr3, eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    dword   es, cs, ss, ds, fs, gs, ldtr;
    word    iobp_off;
    byte    iopb_allow_all[8192];
    byte    iopb_deny_all [8192];
    // I don't have to memset the IOPB now
}CompleteTSS;

#ifndef __PROGRAM_IS_DRIVER

extern void InitIA32(void);
extern word GetInService16(void);

static inline void SetIntVector(Intd *idt, byte v, byte attr, pvoid address)
{
    idt[v].attr = attr;
    idt[v].offset_15_0  = (dword)address &  0xFFFF;
    idt[v].offset_16_31 = (dword)address >> 16;
    idt[v].zero = 0;
}

static inline void MkTrapGate(Intd *idt, byte vector, byte dpl, pvoid address)
{
    byte attr = 0x80 | dpl<<4 | IDT_INT386;
    SetIntVector(idt, vector, attr, address);
}

static inline void MkIntrGate(Intd *idt, byte vector, pvoid address)
{
    byte seg_attr = 0x80 + IDT_INT386;
    SetIntVector(idt, vector, IDT_INT386, address);
}

#endif /* __PROGRAM_IS_DRIVER */

#endif
