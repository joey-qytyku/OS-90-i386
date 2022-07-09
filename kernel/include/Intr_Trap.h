#ifndef INTR_TRAP_H
#define INTR_TRAP_H

#include <Type.h>

enum {
    EXCEPT_DIVZ = 0,
    EXCEPT_DEBUG,
    EXCEPT_NMI,
    EXCEPT_BREAK,
    EXCEPT_OVF,
    EXCEPT_BND,
    EXCEPT_INVOP,
    EXCEPT_DEVNOTAV,
    EXCEPT_DOUBLE_FAULT,
    EXCEPT_SEGOVRN,
    EXCEPT_INVAL_TSS,
    EXCEPT_SEGNPRES,
    EXCEPT_STACK_FAULT,
    EXCEPT_GENERAL_PROTECT,
    EXCEPT_PAGE_FAULT,
    EXCEPT_IMPLEMENTED
};

// For filling the IDT in IA32.c
extern void LowDivide0();
extern void LowDebug();
extern void LowNMI();
extern void LowBreakpoint();
extern void LowOverflow();
extern void LowBoundRangeExceeded();
extern void LowInvalidOp();
extern void LowDevNotAvail();
extern void LowDoubleFault();
extern void LowSegOverrun();
extern void LowInvalidTSS();
extern void LowSegNotPresent();
extern void LowStackSegFault();
extern void LowGeneralProtect();
extern void LowPageFault();

extern void Low0();

// In the assembly file, there are instructions for
// each bottom third ISR that save the IRQ number.
// The opcodes: EB 02 6A 08
//
#define BOTTOM_ISR_TABLE_LEN 4

// Defined in Intr_Trap.asm
extern dword GetErrCode(void);

#endif /* INTR_TRAP_H */
