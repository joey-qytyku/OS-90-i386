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

The FPU uses lazy switching, which means that the FP register context stays until anothe program tries to use them. This improves context switching performance. On newer processors, it is known to be a potential security risk. 

The kernel manipulates FPU registers in C because it does not use them for anything but task switching.

## Modes

There are three modes: user, kernel, and interrupt. The last mode (the one which was just switched from) is stored in a variable. This is used so that the kernel knows if it should save the register dump on the stack to a PCB or simply restore them and continue running the last interrupt or kernel code.

Only the last mode actually matters. The current one is always "known" by the code running and is unimportant.

ESP0 is only used during a ring switch. When this happens, a new stack is loaded. When an interrupt happens on top of an interrupt or kernel code, nothing happens.

## Interrupt interruption

Interrupt handlers (not bottom half) can be interrupted but NOT pre-empted. Other interrupts are completely enabled, but the scheduler is notified to not do any task switching. A critical section must be used to disable them or the handler should be configured to disable them on entry.

The reason INTs can be INTed is because of performance. A slower IRQ handler may slow the whole system if it disables interrupts.

In the kernel API, CriticalSection() will always disable interrupts. EndCritical() will always enable them. The exact implementation is not important, but the operations contained wherein will run as a single unit without any interruption, besides CPU exceptions which cannot be stopped.

# 16-bit DOS Support
## 16-bit tasks versus Kernel Interrupt Calls

The kernel can call interrupt requests and INTx vectors using special functions. The kernel does not do this for 16-bit tasks. A task running in V86 mode does not require any function to enter. Loading the context is sufficient, which includes the EFLAGS register with the VM bit on. This distinction is important.

In both cases, the monitor is used for handling GPF exceptions to emulate ring-0 instructions.

## TSS

The task state segment contains two important fields, ESP0 and SS0. SS0 does not need to change but ESP0 does. The kernel allocates a stack for each program running on the system, both 16-bit and 32-bit. EnterV86 does not re-enter the caller and simply resumes execution in V86 mode. Only an interrupt or exception can stop the execution of any ring-3 code, as well as V86. When GPF is called from V86, the handler is called and ESP0 is loaded from the TSS. Interrupts and exceptions must work in V86 mode or getting out is impossible, but if ESP0 stays the same and the stack is reset upon each supervisor call, the stack of the caller is destroyed and the system will crash. To prevent this, EnterV86 saves ESP to the TSS.

Task switching never happens when the kernel or drivers are running. Interrupts are enabled and time is updated. This is critical for BIOS/DOS calls from protected mode because a task switch will change the kernel stack being used.

## 16-bit tasks

16-bit tasks operate just like 32-bit tasks, except the GPF handler will monitor instructions and emulate them if needed.

# Scheduling algorithm <<<<DEPRECTATED>>>>

I came up with my own algorithm called IOFRQ. It is tied to the virtual memory manager.

The concept is that programs that do a lot of filesystem access are also doing a lot with the data and need boost. Programs that do a lot of algorithmic work without the FS are likely to run slower. There is a workaround for this.

There are three factors: time slice in miliseconds (ts), I/O operations in a time slice (iofrq) and total IO operations in last ts.

iofrq, ts, and tIO are 16-bit numbers.

Time slices are no larger than 1024 miliseconds, or one second. The minimum is 1 MS. The second number cannot be reconfigured but the maximum can be.

iofrq = tIO / TS

Programs that have not done any IO since the last time slice are rewarded with one milisecond. This means that a program that does no IO for increasingly long time slices (impressive) will run very long.
greatly
Becuse iofrq is determined per slice, a program goes in "cool down" once it is done with IO or has slowed down.

As time slices get longer, it is harder for a process to get even more time because the iofrq will begin to stagnate as a result of the division. IO should not be increasing exponentially to match.

## Exploiting the Algorithm <<<<DEPRECTATED>>>>

Programmers should not need to know how schedulers for specific operating systems work. The goal is that the algorithm adapts to the programs and improves performance for certain ones.

Programs that do some IO and some processing are the lowest performing. There is essentially and inverse bell curve for performance. If a program does no IO or a lot of it constantly, it will find it harder to get even more time, so the system is mostly safe from exploits that slow down the system. No process gets too much time.

## Interraction with Virtual Memory <<<<DEPRECTATED>>>>

IOFRQ changes the likelihood that pages from a process may be swapped. VMEM works at a per-process basis.
