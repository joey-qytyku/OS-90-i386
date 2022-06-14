# Scheduler

This document details the handling of interrupts and scheduling of processes.

## Register Dumps

When there is a change from user to interrupt, SS and ESP are definetly on the stack as there was a ring 3-0 transition. The stack looks like:
|Registers|
-|
SS |
ESP |
EFLAGS|
CS|
EIP|

ESP+48 is the start of the interrupt stack frame (without error code).

The general purpose registers are defined in a separate struct and appear on the stack on logical order. This means they are pushed in reverse. ESP is not included.

```c
typedef struct {
  dword eax,ebx,ecx,edx,esi,edi,ebp;
}GeneralRegdump;
```

Entering V86 requires SS and ESP to be set. This is done with a function argument rather than a structure.

Interrupt handlers are given the saved context. They will probably not need it unless it is for software interrupts.

## Context Switching

IRQ 0 has a special upper half C handler which runs the scheduler. It modifies the register dump on the stack to the next process to run and loads floating point environments if needed. When switching, the page directory belonging to the process is placed in CR3 and this PD points the the kernel and user pages.

The FPU is disabled via CR0 and the Device Not Available handler enables the FPU for the processor. CR0 is stored per process. This is because most processes do not need the FPU at all (DOS programs almost never use it).

The kernel manipulates FPU registers in C because it does not use them for anything but task switching.

## Modes

There are three modes: user, kernel, and interrupt. The last mode (the one which was just switched from) is stored in a variable. This is used so that the kernel knows if it should save the register dump on the stack to a PCB or simply restore them and continue running the last interrupt or kernel code.

Only the last mode actually matters. The current one is always "known" and is unimportant.

## Interrupt interruption

Interrupt handlers (not bottom half) can be interrupted but NOT pre-empted. Other interrupts are completely enabled, but the scheduler is notified to not do any task switching. A critical section must be used to disable them or the handler should be configured to disable them on entry.

The reason INTs can be INTed is because of performance. A slower IRQ handler may slow the whole system if it disables interrupts.

In the kernel API, CriticalSection() will always disable interrupts. EndCritical() will always enable them. The exact implementation is not important, but the operations contained wherein will be garaunteed to be atomic.

Todo: When are drivers initialized?

# Scheduling algorithm

I came up with my own virtual memory algorithm called IOFRQ. It is tied to the virtual memory manager.

The concept is that programs that do a lot of filesystem access are also doing a lot with the data and need boost. Programs that do a lot of algorithmic work without the FS are likely to run slower. There is a workaround for this.

There are three factors: time slice in miliseconds (ts), I/O operations in a time slice (iofrq) and total IO operations in last ts.

iofrq, ts, and tIO are 16-bit numbers.

Time slices are no larger than 1024 miliseconds, or one second. The minimum is 1 MS. The second number cannot be reconfigured but the maximum can be.

iofrq = tIO / TS

Programs that have not done any IO since the last time slice are rewarded with one milisecond. This means that a program that does no IO for increasingly long time slices (impressive) will run very long.

Becuse iofrq is determined per slice, a program goes in "cool down" once it is done with IO or has slowed down.

As time slices get longer, it is harder for a process to get even more time because the iofrq will begin to stagnate as a result of the division. IO should not be increasing exponentially to match.

## Exploiting the Algorithm

Programmers should not need to know how schedulers for specific operating systems work. The goal is that the algorithm adapts to the programs and improves performance for certain ones.

Programs that do some IO and some processing are the lowest performing. There is essentially and inverse bell curve for performance. If a program does no IO or a lot of it constantly, it will find it harder to get even more time, so the system is mostly safe from exploits that slow down the system. No process gets too much time.

## Interraction with Virtual Memory

IOFRQ changes the likelihood that pages from a process may be swapped. VMEM works at a per-process basis.
