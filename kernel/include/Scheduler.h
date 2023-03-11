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

#define PID DWORD

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

#define LOCAL_V86INT_REFLECT

//
// Exception handlers use the registers instead of the stack
//
// The first parameter is a DWORD that is 1 if the context is V86
// This is important for #GP
//
#define CREATE_EXCEPT_HANDLER(name) __attribute__(( regparm(2) )) void name

typedef struct __PACKED __ALIGN(4)
{
    DWORD   handler_address;
    WORD    handler_code_segment;
    WORD    type;
}LOCAL_PM_IDT_ENTRY;

//
// The Process control block. Very large structure.
//
typedef struct __PACKED __ALIGN(4)
{
    // Switches between processes will always change rings
    DWORD   context[RD_NUM_DWORDS];
    DWORD   kernel_pm_stack;

//
// The kernel must use a local stack for each DPMI
// process when handling real mode calls.
// The stack pointer must also be remembered because interrupts
// can occur during the handling of a real mode INT
//
    WORD    kernel_real_mode_ss;
    WORD    kernel_real_mode_sp;

//
// Flags related to the process
//
    BYTE    thread32:1;// Thread is in protected mode
    BOOL    run:1;     // Is structure valid
    BOOL    use87:1;   // Does thread use x86 FPU
	PVOID   x87env;    // NULL if no 87

// If NULL, no sub-process is running
// If != NULL, the scheduler will not run the parent program
// and run the subprocess (recursively going down)
    PVOID   subproc;

//
// Local protected mode trap entries, see docs for more info
//
    LOCAL_PM_IDT_ENTRY local_idt[256];


    PVOID   next;      // Front link to next thread
}THREAD,*PTHREAD;

extern VOID InitScheduler(VOID);

#endif /* SCHEDULER_H */
