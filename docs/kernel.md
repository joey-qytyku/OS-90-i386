# Kernel Overview

This document describes the purpose of the kernel, which is called KERNL386.SYS. It is marked as a system file and is loaded at physical address 0x110000 and virtual address.

The kernel binary is position independent and runs at the virtual address of C0000000.

# Fileystem and Disk Access

Upon startup, filesystem and disk access is immediately possible through the V86 mode interface. Drivers can trap real mode interrupts and IRQs to implement 32-bit disk access.

[Service1] -> [V86] -> [DOS]
[Service2] -> [DRV]

# Exiting to DOS

The user can exit to DOS by exiting the command prompt. When the first process terminates,
