#ifndef IA32_H
#define IA32_H

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

#define TSS_ALLOW_OFFSET 104
#define TSS_DENY_OFFSET  (104+8192)

// The data/stack segment enables the BIG bit
// so that Plug-and-play BIOS recognizes it as
// a 32-bit stack
#define TYPE_TSS  0x19
#define TYPE_DATA 0x12
#define TYPE_CODE 0x1B /* Readable for PnP */
#define ACCESS_RIGHTS(present, ring, type)\
(present<<7 | ring<<6 | type)

#define LDT_SIZE 16

enum {
GDT_KCODE,
GDT_KDATA,
GDT_UCODE,
GDT_UDATA,
GDT_TSSD,
GDT_PNPCS,
GDT_PNP_OS_DS,
GDT_PNP_BIOS_DS,
GDT_ENTRIES
};

typedef DWORD PAGE;
///////////////////////////
//    Data structures    //
///////////////////////////

typedef struct __PACKED
{
    WORD    offset_15_0;
    WORD    selector;
    BYTE    zero;
    BYTE    attr;
    WORD    offset_16_31;
    // Trap gates use the DPL
    // Interrupt gates do not
}INTERRUPT_DESCRIPTOR,
*PINTERRUPT_DESCRIPTOR;

typedef struct __PACKED
{
    WORD    limit
    WORD    base0;
    BYTE    base1;
    BYTE    access;
    BYTE    limit_gr;
    BYTE    base2;
}SEGMENT_DESCRIPTOR,
*PSEGMENT_DESCRIPTOR;

// The standard register dump, ESP is nonsense
typedef struct __PACKED {
    DWORD   eax, ebx, ecx, edx, esi, edi, ebp, __esp;
}REGS_IA32,*PREGS_IA32;

// When a task switch takes place, the CPU
// pushes these values on the ESP0 stack
// ESP+48 is the start of the trap frame
//
typedef struct __PACKED {
    DWORD       eip,cs,eflags,ss,esp;
    REGS_IA32   regs; // The lower-half handler saves these
}TRAP_FRAME,*PTRAP_FRAME;

typedef struct __PACKED
{
    WORD    limit;
    DWORD   address;
}DESCRIPTOR_REGISTER;

typedef struct __PACKED {
    DWORD   link; // Zero extended
    DWORD
        esp0, ss0,
        esp1, ss1,
        esp2, ss2;
    DWORD   cr3, eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    DWORD   es, cs, ss, ds, fs, gs, ldtr;
    WORD    iobp_off; // +103
    BYTE    iopb_allow_all[8192]; // +104
    BYTE    iopb_deny_all [8192];
    // I don't have to memset the IOPB now
}COMPLETE_TSS;

typdef struct {
    COMPLETE_TSS            tss;
    SEGMENT_DESCRIPTOR      gdt[GDT_ENTRIES];
    SEGMENT_DESCRIPTOR      ldt[LDT_SIZE];
    INTERRUPT_DESCRIPTOR    idt[256];
}IA32_STRUCT,*PIA32_STRUCT;

/////////////////////////////////
//           Externs           //
/////////////////////////////////
extern VOID InitIA32     (VOID);
extern VOID AppendAddress(PVOID,DWORD);

extern IA32_STRUCT _ia32_struct;

#define IaIOPB_Allow() { _ia32_struct.tss.iobp_off = TSS_ALLOW_OFFSET; }
#define IaIOPB_Deny()  { _ia32_struct.tss.iobp_off = TSS_ALLOW_OFFSET; }

// Cannot use AppendAddress for IDT because offset
// is two separate words
#define SetIntVector(ia32, vector, attr, address)
    ia32.idt[vector].attr = attr;\
    ia32.idt[vector].offset_15_0  = (DWORD)address &  0xFFFF;\
    ia32.idt[vector].offset_16_31 = (DWORD)address >> 16;\
    ia32.idt[vector].zero = 0;

#define MkTrapGate(idt, vector, dpl, address) \
    SetIntVector(idt, vector, 0x80 | dpl<<4 | IDT_INT386, address);

#define MkIntrGate(idt, vector, address)\
    SetIntVector(idt, vector, 0x80 | IDT_INT386, address);

// A.k.a BIOS selector
#define PnSetBiosDsegBase(base)\
    AppendAddress(&_ia32_struct.gdt[GDT_PNP_BIOS_DS], base);

#define PnSetOsDsegBase(base)\
    AppendAddress(&_ia32_struct.gdt[GDT_PNP_OS_DS], (DWORD)base);

#define PnSetBiosCsegBase(base)\
    AppendAddress(&_ia32_struct.gdt[GDT_PNPCS], (DWORD)base);

#endif /* IA32_H */
