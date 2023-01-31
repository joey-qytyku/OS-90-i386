# Kernel Overview

This document describes the purpose of the kernel, which is called KERNL386.EXE.

The kernel binary is position independent and runs at the virtual address of C0000000.

# Design Concept

The idea is that all requests to hardware from the userspace will be handled by a component on a stack of software following the layered model. Because DOS is a fully featured operating system, its interface is used, but it is extended to allow the 32-bit software to control it or pass it down the stack. Almost every request to the system is able devolve to a DOS driver/API call or BIOS function, ensuring high compatibility at the expense of slight overhead.

32-bit software can also function as an abstraction layer on the stack, e.g. USB host and USB device.
```
High Level
[  User Request  ]=\\
[  OS/90 Kernel  ]-||
[ Driver |   Bus ]-||
[   DOS Kernel   ]-||
[  PC BIOS traps ] \/
Low level
```
# Fileystem and Disk Access

Upon startup, filesystem and disk access is immediately possible through the V86 mode interface. Drivers can trap real mode interrupts and IRQs to implement 32-bit disk access.

[Service1] -> [V86] -> [DOS]
[Service2] -> [DRV]

# Exiting to DOS

Exiting to DOS is impossible is OS/90. I could not figure it out, and do not see a huge advantage.
