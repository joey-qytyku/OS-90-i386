# Scheduler

## Context Switching

IRQ 0 has a special upper half C handler which runs the scheduler. It modifies the register dump on the stack to the next process to run and loads floating point environments if needed. When switching, the page directory belonging to the process is placed in CR3 and this PD points the the kernel and user pages.

The FPU is disabled via CR0 and the Device Not Available handler enables the FPU for the processor. CR0 is stored per process. This is because most processes do not need the FPU at all (DOS programs almost never use it).

The kernel anipulates FPU registers in C because it does not use them for anything but task switching.

## Modes

There are three modes: user, kernel, and interrupt. The last mode (the one which was just switched from) is stored in a variable. This is used so that the kernel knows if it should save the register dump on the stack to a PCB or simply restore them and continue running the last interrupt or kernel code.

## Kernel pre-emption

The kernel CANNOT be pre-empted, but interrupts can be. 

## Interrup interruption

Interrupt handlers (not bottom half) can be interrupted but NOT pre-empted. Other interrupts are completely enabled, but the scheduler is notified to not do any task switching. A critical section must be used to disable them.

In the kernel API, CriticalSection() will always disable interrupts. EndCritical() will always enable them.
