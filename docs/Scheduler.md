# Scheduler

# Virtual 86 Machines

VM86 is used by the kernel for information gathering through the BIOS. Virtual 86 machines run as processes on the system. A SSM is atomic and synchronous while executing. A general purpose virtual machine runs when the scheduler decides to run it.

## Design

Each VM has a process control block like all other processes. An identifier specifies that it is an 86 machine. A GPM acts like a process, but switching to it is different.

Two task state segments are used by the kernel. The second only is for VM86.

The GP# handler looks at the process that was running and if it was a VM, it may emulate the instruction that caused it. Otherwise, it terminates the process.

## Interrupt Faking

Interruptss go through the GP# handler rather than IDT entries (unless IOPL is 3, not in this case). THe GP# handler emulates INT by manipulating the stack frame.

During startup, the kernel must run 16-bit drivers if there is no alternative.

## Instruction Emulation

Prefixes act as instrutions and the prefixed instruction can be interrupted before it executes.

## System Service Machine

A SSM runs synchronously. It is stored by the kernel with a PCB and runs atomically. This machine is only used for running BIOS interrupts or running a 16-bit driver.

## General Purpose Machine

GPMs can be configured to directly access IO ports, video memory, and the BIOS, but are designed for user programs that only use the DOS API. They are also allowed to change video modes.

A GPM can access DOS interrupts and can be spawned by a userspace process with all of its memory being accessible to the parent process through.

# 32-bit Virtual Machines

Native applications use the PE format. They get 1G of bottom half addressing space. The FPU context is only saved if
