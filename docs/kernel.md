# Kernel Overview

This document describes the purpose of the kernel, which is called KERNL386.SYS. It is marked as a system file and is loaded at the top of 1M.

The kernel binary is position independent and runs at the base address of C0000000.

# Built in Drivers

These drivers are in the kernel.

ATA and ATAPI
AT/PS2 keyboard (8042), anychronous and synchronous I/O
Floppy disk using DMA and PIO
System clock

# Fileystem

The filesystem is 16-bit by default and goes through DOS. INT 13H is intercepted by the kernel to access 32-bit drivers. Logical disks are organized in partition order.

The userspace and kernel mode API for accessing
