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

enum {
    KERNEL = 0,
    INTERRUPT, /* If an INT gets INTed */
    USER
};

// If the program calls INT with this, reflect to DOS
// using the global capture chain
// The default for DOS except for INT 21H and INT 31H
#define LOCAL_PM_INT_REFLECT_GLOBAL 0

//
// Same as last but it will disable interrupts upon entry.
//
#define LOCAL_PM_IRQ_REFLECT_GLOBAL 1

// This interrupt vector points to an ISR for a fake IRQ
// It should not be called by INT or program crashes
// Interrupts that normally would be for DOS, (Ranges 8H and 70h)
// Point to where they would be expected to.
#define LOCAL_PM_INT_IDT_FAKE_IRQ 2

// This interrupt vector was modified by the program with a PM trap handler
// If INT is called, it is caught by the #GP handler and passed to the DOS
// server for emulation.
#define LOCAL_INT_PM_TRAP 3

union __ALIGN(4)
{
    DWORD   handler_address;
    WORD    handler_code_segment;
    WORD    type;
}LOCAL_IDT_ENTRY;

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

extern VOID InitScheduler(VOID);

#define PID DWORD

#endif /* SCHEDULER_H */
