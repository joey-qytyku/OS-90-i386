/*
     This file is part of OS/90.

    OS/90 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

    OS/90 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with OS/90. If not, see <https://www.gnu.org/licenses/>. 
*//*

2022-07-08 - Refactoring, changed inlines to #defines, fixed gdt struct

*/

#ifndef IA32_H
#define IA32_H

#include <Type.h>

typedef DWORD PAGE;

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
/* Readable for PnP */
#define TYPE_CODE 0x1B
#define ACCESS_RIGHTS(present, ring, type) (present<<7 | ring<<6 | type)

#define LDT_SIZE 16

///////////////////////////
//   Segments and IDT    //
///////////////////////////

#define SetIntVector(vector, _attr, address)\
    _ia32_struct.idt[vector].attr = _attr;\
    _ia32_struct.idt[vector].offset_15_0  = (DWORD)address &  0xFFFF;\
    _ia32_struct.idt[vector].offset_16_31 = (DWORD)address >> 16;\
    _ia32_struct.idt[vector].zero = 0;

#define MkTrapGate(vector, dpl, address)\
    SetIntVector(vector, 0x80 | dpl<<4 | IDT_INT386, address);

#define MkIntrGate(vector, address)\
    SetIntVector(vector, 0x80 | IDT_INT386, address);

#define PnSetBiosDsegBase(base)\
    AppendAddress(&_ia32_struct.gdt[GDT_PNP_BIOS_DS], base);

// Plug and play related

#define PnSetOsDsegBase(base)\
    AppendAddress(&_ia32_struct.gdt[GDT_PNP_OS_DS], (DWORD)base);

#define PnSetBiosCsegBase(base)\
    AppendAddress(&_ia32_struct.gdt[GDT_PNPCS], (DWORD)base);

#define IaIOPB_Allow() _ia32_struct.tss.iobp_off = TSS_ALLOW_OFFSET;
#define IaIOPB_Deny()  _ia32_struct.tss.iobp_off = TSS_ALLOW_OFFSET;

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
    WORD    limit;
    WORD    base0;
    BYTE    base1;
    BYTE    access;
    BYTE    limit_gr;
    BYTE    base2;
}SEGMENT_DESCRIPTOR,
*PSEGMENT_DESCRIPTOR;

// The standard register dump, ESP is nonsense
typedef struct __PACKED
{
    DWORD   eax, ebx, ecx, edx, esi, edi, ebp, _esp;
}REGS_IA32,*PREGS_IA32;

// When a task switch takes place, the CPU
// pushes these values on the ESP0 stack
// ESP+48 is the start of the trap frame
//
typedef struct __PACKED
{
    DWORD       eip,cs,eflags,ss,esp;
    REGS_IA32   regs; // The lower-half handler saves these
}TRAP_FRAME,*PTRAP_FRAME;

typedef struct __PACKED
{
    WORD    limit;
    DWORD   address;
}DESCRIPTOR_REGISTER;

typedef struct __PACKED
{
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

typedef struct __attribute__(( aligned(64) ))
{
    SEGMENT_DESCRIPTOR      gdt[GDT_ENTRIES];
    SEGMENT_DESCRIPTOR      ldt[LDT_SIZE];
    INTERRUPT_DESCRIPTOR    idt[256];
    COMPLETE_TSS            tss;
}IA32_STRUCT,*PIA32_STRUCT;

/////////////////////////////////
//           Externs           //
/////////////////////////////////

extern VOID   InitIA32     (VOID);
extern VOID   AppendAddress(PVOID,DWORD);
extern IA32_STRUCT _ia32_struct;

#endif /* IA32_H */

