#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Platform/IA32.h>
#include <Type.h>

#define VM_32 0
#define VM_16 1

typedef enum {
    KERNEL = 0,
    INTERRUPT, /* If an INT gets INTed */
    USER
}Mode;

typedef struct __ALIGN(4)
{
    // Switches between processes will always change rings
    TrapFrame context;
    dword   kernel_stack; // ?
    byte    thread32;  /* Thread is 32-bit native code */
    bool    run;       /* Is structure valid        */
    bool    use87;     /* Does thread use x86 FPU   */
	pvoid   x87env;    /* NULL if use87 false       */
 	short   ts;        /* Miliseconds left counter  */
    bool    ioperm;    /* IO permission bitmap      */
    pvoid   next;      /* Front link to next thread */
}Thread,*PThread;

#ifndef __PROGRAM_IS__DRIVER

extern void InitScheduler(void);

#endif /* !__PROGRAM_IS__DRIVER */

#endif /* SCHEDULER_H */
