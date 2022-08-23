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
};

typedef struct __ALIGN(4)
{
    // Switches between processes will always change rings
    TRAP_FRAME context;
    DWORD   kernel_stack; // ?
    BYTE    thread32;  /* Thread is 32-bit native code */
    BOOL    run;       /* Is structure valid        */
    BOOL    use87;     /* Does thread use x86 FPU   */
	PVOID   x87env;    /* NULL if use87 false       */
 	WORD    ts;        /* Miliseconds left counter  */
    BOOL    ioperm;    /* IO permission bitmap      */
    PVOID   next;      /* Front link to next thread */
}THREAD,*PTHREAD;

#ifndef __PROGRAM_IS__DRIVER

extern VOID InitScheduler(VOID);

#endif /* !__PROGRAM_IS__DRIVER */

#endif /* SCHEDULER_H */
