#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Type.h>
#include <IA32.h>

#define VM_32 0
#define VM_16 1

typedef struct __attribute__((aligned (4)))
{
	long eax,ebx,ecx,edx,esi,edi,ebp,esp3,esp0;
	dword eip,eflags;
	byte VMType;
	short TimeSliceMS;
	void *x87env;
	bool active;
    bool ;
}ProcCtlBlk; // Alignment ensures fast access

static inline void ClearInts(void) { __asm__ volatile("cli"); }
static inline void SetInts  (void) { __asm__ volatile("sti"); }

typedef enum {
    KERNEL,
    INTERRUPT, // If an INT gets INTed
    USER
}Mode;

#endif
