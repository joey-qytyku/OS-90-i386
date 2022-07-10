#ifndef V86_H
#define V86_H

#include <Platform/IA32.h>
#include <Type.h>

extern void EnterV86(PTrapFrame);
extern void ShootdownV86(void);

#endif /* V86_H */
