/*
     This file is part of OS/90.

    OS/90 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

    OS/90 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with OS/90. If not, see <ttps://www.gnu.org/licenses/>.
*/

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
}SCHEDULER_MODE;

typedef struct __ALIGN(4)
{
    // Switches between processes will always change rings
    TRAP_FRAME context;
    DWORD   kernel_stack;
    BYTE    thread32;  // Thread is 32-bit native code
    BOOL    run;       // Is structure valid
    BOOL    use87;     // Does thread use x86 FPU
	PVOID   x87env;    // NULL if no 87
 	WORD    ts;        // Miliseconds left counter
    BOOL    ioperm;    // IO permission bitmap
    PVOID   next;      // Front link to next thread
}THREAD,*PTHREAD;

#ifndef __PROGRAM_IS__DRIVER

extern VOID InitScheduler(VOID);

#endif /* !__PROGRAM_IS__DRIVER */

#endif /* SCHEDULER_H */
