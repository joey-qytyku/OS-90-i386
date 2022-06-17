# 16-bit virtual machines

Multitasking VM86

# Virtual Machine manager

VMMs are a special type of driver. There can only be one running on a system. This driver is responsible for terminating and creating virtual machines configuring resources etc, interrupts. The kernel provides a simple interface that makes this possible.

TSR programs do not work.

# Restrictions

# Interrupt Calls and IRQs

An IRQ handler that is 16-bit reclaimable will be executed by the master IRQ handler using the V86 interface.

# The kernel

The kernel is 32-bit but needs to drop into DOS to perform file IO and trap out of it to do 32-bit disk IO. A VM is not created for th

## Interaction with scheduler

When the kernel runs in V86 mode, interrupts are enabled and are always caught by the IDT. This means that the kernel can preempt programs running in this mode unless interrupts are disabled prior.

# Implementation of V86 Monitor

The TSS is never modified by the monitor or the basic V86 interface. It is modified at a per-process basis by the scheduler.

Each task gets a supervisor stack. When the kernel handles exceptions and interrupts, it can discard the stack. When the next int/except happens, it will use the same stack specified in the TSS, which is modified for each task appropriately.

The V86 monitor is called by the GPF handler. When it returns, the real mode code will continue to run.

See more in scheduler.md

Stacks, TSS?

# Configuration of VM16 tasks

The resource manager determines which interrupts are 16-bit or 32-bit. 16-bit IRQs are serviced in the physical DOS and only apply to the OS when it performs DOS/BIOS calls. The kernel capture table is for software interrupts. It can send a software interrupt back to itself or 

Each 16-bit process has its own local capture table.

The function GlobalMap(dword pid, void *proc_page, void *to, page c, int access) can be used to change the mappings of pages in V86 tasks as well as 32-bit tasks. This can be used to facilitate IPC for VM32 and allow for accessing framebuffers.

GlobalMap(thepid, 0xB8, framebuffer, 16, PG_RW);

# API

Int16()
SetGlobalCapture(byte vector, byte type)

#define SWI_16 0
#define SWI_32 1

# XMS Emulation

HIMEM.SYS is never used by the kernel. The kernel emulates the latest XMS specification so that DOS programs go through the 32-bit memory manager. TSR programs may use HMBs, so there is no way to override this.

XMS does not use an interrupt. It uses 

The A20 functions always respond with the A20 gate being on.
