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


// Low exception handlers, explained in Intr_Trap.h
// Not inserted individually, copied
// from a function pointer array
//
extern void Low0();
extern void Low1();
extern void Low2();
extern void Low3();
extern void Low4();
extern void Low5();
extern void Low6();
extern void Low7();
extern void Low8();
extern void Low9();
extern void Low10();
extern void Low11();
extern void Low12();
extern void Low13();
extern void Low14();
extern void Low15();


// Defined in Intr_Trap.asm
extern dword GetErrCode(void);

#endif /* INTR_TRAP_H */
