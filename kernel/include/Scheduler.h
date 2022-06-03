#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Type.h>
#include <IA32.h>

#define VM_32 0
#define VM_16 1

typedef enum {
    KERNEL,
    INTERRUPT, /* If an INT gets INTed */
    USER
}Mode;

typedef struct __ALIGN(4)
{
// add context
	byte    VMType;
	bool    run;        /* Is structure valid       */
    bool    use87;      /* Does thread use x86 FPU  */
	void    *x87env;    /* */
 	short    ts;        /* Miliseconds left counter */
    bool    *ioperm;    /* IO permission bitmap */
    void *next;         /* Front link to next thread     */
}Thread,*PThread;       /* Alignment ensures fast access */

void InitScheduler(void);
static inline void ClearInts(void) { __asm__ volatile("cli"); }
static inline void SetInts  (void) { __asm__ volatile("sti"); }

#endif
