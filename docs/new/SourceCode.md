# Source Code Overview

The kernel contains the following components:

* Early initialization (Main.c, StartK.asm)
* Architectural init. (IA32.c)
* V86 interface (V86.c, V86.asm)
* Resource management (Resource.c)
* Memory management (Memory.c)


IA32.c implements functions related to the GDT, IDT, TSS, etc. The header file defines several macros. Functions that are not available to drivers are #if'ed out. This file includes basically everything so that it can initialize the system.

Scheduler.c controls exceptions, interrupts, and pre-emptive scheduling of programs. It is the home of the master IRQ handler. This file uses IA32.h for IO inlines and V86.h for reflecting 16-bit interrupts. It directly interfaces with the PIC.

Intr_Trap.h implements low exception handlers. It defines GetErrCode which returns the error code stored in a file-local variable as well as.
