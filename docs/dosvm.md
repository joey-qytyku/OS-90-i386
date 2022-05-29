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

# API

# XMS Emulation

HIMEM.SYS is never used by the kernel. The kernel emulates the latest XMS specification so that DOS programs go through the 32-bit memory manager.

The A20 functions always respond with the A20 gate being on.
