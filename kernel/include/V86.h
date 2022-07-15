#ifndef V86_H
#define V86_H

#include <Platform/IA32.h>
#include <Type.h>

extern void ScMonitorV86(PTrapFrame);
extern void ScEnterV86(PTrapFrame);
extern void ScShootdownV86(void);     // Defined in vm86.asm

#endif /* V86_H */
