# KuDOS Driver Model

KDM is the system used to implement drivers. A driver is a program that runs in ring zero and has access to the kernel API. Drivers can be of the following types:

* Graphics driver
* Block device driver
* Character device driver



# Executable and Loading

kuDOS drivers end with the extension .DRV and are PE/coff objct files. This is because they contain API refferences and must be relocated to the kernel address space. The subsystem of the executable is unimportant, but should be console. The base address of the object file can be anything, but 1M is recommended if real mode virtual machines are to be used.

KuDOS drivers are loaded in the upper half address space region. They are loaded above the kernel and do not overlap. kuDOS drivers must have at least a .text section. In physical memory, the drivers are loaded in memory in a fragmented manner.

Drivers are initialized at startup but cannot be unloaded without restarting the operating system. 

## Driver descriptor structure

The DDS is a structure present anywhere in the execuable.

# Kernel Symbol Table

The KST is an allocated region of memory containing public 

# Types of drivers

## Graphics driver

A graphics driver is used for pixel graphics. It exposes functions used for drawing pixels and setting video modes. Multiple can be loaded, but only one should be used.
