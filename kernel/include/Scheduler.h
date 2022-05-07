#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Type.h>

#define VM_32  0  // 32-bit process
#define VM_16U 1 // User/Concurrent VM
#define VM_16S 2 // Supervisor/Atomic VM

//extern int GeneralProtect;

typedef struct __attribute__((aligned (4)))
{
	long eax,ebx,ecx,edx,esi,edi,ebp,esp3,esp0;
	dword eip,eflags;
	byte VMType;
	short TimeSliceCounterMS;
	void *x87Env;
}ProcCtlBlk; // Alignment ensures fast access

static inline void ClearInts(void) { __asm__ volatile("cli"); }
static inline void SetInts  (void) { __asm__ volatile("sti"); }

#endif