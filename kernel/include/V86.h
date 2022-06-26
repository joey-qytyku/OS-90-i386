#ifndef V86_H
#define V86_H

#include <Type.h>
#include <IA32.h>

extern void EnterV86(PTrapFrame);
extern void ShootdownV86(void);

#endif /* V86_H */
