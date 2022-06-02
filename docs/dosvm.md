# 16-bity virtual machines

Multitasking VM86

# Virtual Machine manager

VMMs are a special type of driver. There can only be one running on a system. This driver is responsible for terminating and creating virtual machinesmm configuring resources etc,interrupts. The kernel provides a simple interface that makes this possible.

TSR programs do not work.

# Restrictions

The kernel allows DOS VMs to modify interrupt vectors using the irq request feature. A VMM should ask the user if this is okay as DOS viruses often hook interrupts.

# The kernel

The kernel is 32-bit but needs to drop into DOS to perform file IO and trap out of it to do 32-bit disk IO. A VM is not created for this purpose.

## Interaction with scheduler

When the kernel runs in V86 mode, interrupts are enabled and are always caught by the IDT. This means that the kernel can preempt programs running in this mode unless interrupts are disabled prior.

# Implementation of V86 Monitor

The TSS is never modified by the monitor or the basic V86 interface. It is modified at a per-process basis by the scheduler.

# Configuration of VM16 tasks

The resource manager determines whihc interrupts are 16-bit or 32-bit. 16-bit interrupts are serviced in the global DOS.

The function GlobalMap(dword pid, void *proc_page, void *to, page c, int access) can be used to change the mappings of pages in V86 tasks as well as 32-bit tasks. This can be used to facilitate IPC for VM32 and allow for accessing framebuffers.

GlobalMap(thepid, PTR(0xB8), framebuffer, 16, PG_RW);

# API

# XMS Emulation

HIMEM.SYS is never used by the kernel. The kernel emulates the latest XMS specification so that DOS programs go through the 32-bit memory manager. TSR programs may use HMBs, so there is no way to override this.

XMS does not use an interrupt. It uses 

The A20 functions always respond with the A20 gate being on.
