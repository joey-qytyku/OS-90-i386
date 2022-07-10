# OS/90 Readme

This project is an operating system designed for vintage x86 computers. It is written in assembly but supports the C language.

## Minimal Requirements

2MB of RAM and an i386SX are the minimum. A i486DX with 4MB with plug-and-play BIOS support is recommended to use the full feature set.

If there is no plug-and-play BIOS, bus and device drivers cannot use the PnP features and must rely on static IRQ assignments provided by the user to the driver. Alternatively, 16-bit DOS drivers can be used, although they are less stable.

## Installation

All files are in the OS90 directory, which can be renamed. The driver GRABIRQ.SYS must be loaded FIRST in CONFIG.SYS.

## General Information

OS/90 is dependent on an MS-DOS compatible operating system for bootstrapping. It is able to run programs designed for DOS and the 32-bit userspace. 32-bit Drivers can replace 16-bit real mode ones and are provided an API.

Features:
* 32-bit disk access, filesystem, and caching
* DOS compatibility
* Drivers for ISA and PCI buses
* Source code is public domain


## Problems
