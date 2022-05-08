# Kernel Overview

This document describes the purpose of the kernel, which is called KERNL386.SYS. It is marked as a system file and is loaded at the top of 1M.

The kernel binary is position independent and runs at the virtual address of C0000000.

# Built in Drivers

These drivers are in the kernel.

ATA and ATAPI using programmed I/O
AT/PS2 keyboard (8042), anychronous and synchronous I/O
Floppy disk using DMA and PIO
System clock and RTC

This hardware is standardized equipment and is not shared with any other modules. Some of it may be moved out of the base kernel.

# Overall Design

OS/90 mostly follows an API-driven layered model.

Besides a few standard devices, the kernel does not do much direct hardware access. The kernel's job is to perform pre-emptive, reentrant multitasking, as well as task termination and execution. 32-bit programs communicate with the operating system through userspace libraries (WIP). 16-bit DOS programs communicate with DOS and the kernel performs arbitration of resources, as there is only one DOS kernel. I consider the kernel to be monolithic because of the built in drivers.

Drivers are program that run in ring 0 and have complete control. They are essentially extentions to the kernel. Drivers can take ownership of resources like IRQs, DMA channels, and memory mapped IO. Whenever these resources are accessed, they can be notified and perform the operation on the caller's behalf. If an IRQ is owned by a real-mode driver, it can be replaced.

# Interrupt Handling

There is one same x86 ISR on every IRQ vector. The source of the interrupt is deduced by reading the in-service register from the 8259A PIC. This goes to the VINT system, which dispatches interrupt appropiately to their owners or to the kernel. The exception is for the timer, which is handled by a separate IRQ.

Interrupt sharing is supported (TODO) for devices that use the PCI bus and allows multiple drivers to own the same interrupt (assign an ID?). If the PCI bus is unavailable, then only one driver can run on a single IRQ vector.

Once an interrupt becomes 32-bit, it cannot be 16-bit again and is replaced.

TODO: Make it so that ISA interrupts and resources can be taken by drivers.

# DOS drivers

16-bit drivers are mostly usable in OS/90. They should be executed in autoexec.bat rather than in the command line. Real-mode drivers should be avoided because they may be unstable, but are unavoidable as there probably will not be many drivers made for OS/90.

.SYS drivers not natively supported by OS/90, but should work okay under a DOS VM. Avoid block device drivers because disk access is 32-bit anyway (VDISK may work). HIMEM.SYS is required to boot and most .SYS drivers do no harm. If they hook ATA interrupts, then it simply will not do anything because disk access is always 32-bit. Using a SYS file for the mouse, for example, may actually work, but MOUSE.COM is a better idea. The Microsoft mouse driver (or ctmouse) will hook IRQ 13, which by default is redirected to DOS. Only DOS programs or some kernel-mode driver using a virtual machine can access the mouse multiplex API.

To be more specific, drivers which implement block/character device transfers will certainly not work (ANSI, VDISK). They will just be ignored. VDISK using EMBs to store data, which are not overwritten by the kernel (which also lives in an EMB). Be caeful though and back up any data in a ramdrive before running 90.COM to be safe.

# Fileystem

The filesystem is 16-bit by default and goes through DOS. INT 13H is intercepted by the kernel to access 32-bit drivers. Logical disks are organized in partition order.

# Exiting to DOS

The user can exit to DOS by exiting the command prompt. When the first process terminates,
