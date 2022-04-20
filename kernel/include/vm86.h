/*
 * For kernel use only
 */


#include <Type.h>

struct VMControl
{
	byte conf_bytes[16];
	word memblock4k;
};

#define VM16_C0_PREEMPT 1

#define VM16_C1_ENVIDEO 1
#define VM16_C1_CHVMODE 2

extern void AsmSimulateInterrupt(int v);
