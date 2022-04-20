# OS/90 Driver Model

KDM is the system used to implement drivers. A driver is a program that runs in ring zero and has access to the kernel API. Drivers can be of the following types:

1. Graphics driver
2. Block device driver
3. Character device driver
4. Privilege Escalator

# Executable and Loading

OS/90 drivers end with the extension .DRV and are PE/coff object files. This is because they contain API refferences and must be relocated to the kernel address space. The subsystem of the executable is unimportant, but should be console. The base address of the object file can be anything, but 1M is recommended if real mode virtual machines are to be used.

OS/90 drivers are loaded in the upper half address space region. They are loaded above the kernel and do not overlap. OS/90 drivers must have at least a .text and .dds section.

Drivers cannot execute their data and bss sections but can modify the code section.

Drivers are initialized at startup but cannot be unloaded without restarting the operating system. 

## Driver descriptor structure

The DDS is a structure present at the start of the .text section. It is defined this way:


|Type|Description|
-|-|-
DWORD| Magic number (0x2BADFADA)
DWORD| Initialization entry point
DWORD| Driver call entry point
BYTE | Driver type (see intro)

The driver entry point and the init entry point are relative to the start of the object file and are relocatable addresses.

# Kernel Symbol Table

The KST is a dynamic region of memory that stores pointers to kernel API functions. It functions independently of userspace API functions, which are unavailable to kernel and drivers.

The KST is dynamically allocated so that drivers can add their own functions. Variables can also be added to the KST.

Each KST entry contains an absolute virtual address and a 28-character null-terminated string with the symbol name. Names can be up to 27 characters long.

When drivers are loaded, the KST is applied. Undefined references to kernel symbols are replaced. Drivers can update the KST. It does not need to store addresses and can store other 32-bit integers.

# Plug and Play ISA Support and resource management

Resources are tracked by the kernel. IRQs, ISA DMA chanels, memory addresses, and IO spaces are kept track of in dynamically allocated structures. Port ranges are 4-byte aligned for PCI compatibility. Plug and play ISA features are not included in the kernel and will likely be a separate feature in the future. PCI support is built in. int 0x1A is used to detect PCI by the bootloader.

Drivers will be given the ability to request resources, but the kernel cannot garauntee them. A sound card has to send IRQs rapidly and may need to minimize interrupt sharing. IRQ 5 is the best option and most common configuration. PCI allows interrupt lines to be re-assigned.

Sound blaster 16 PCI can assign the IRQ on its own using a command. (Research this)

# Types of drivers

## Graphics driver

A graphics driver is used for pixel graphics. It exposes functions used for drawing pixels and setting video modes. Multiple can be loaded, but only one should be used.

## Privilege Escalator

These types of drivers allow userspace programs to access low-level functions that they do not have privileges for, in a way that can be monitored and restricted for stability. PED's do not use the driver entry point.

An example of a privilege escalator could be a DOS VM manager that is then used by a command shell to execute DOS programs.


# KDM API

The KDM API provides a complex interface with low-level and high-level functions. Each of these functions uses explicit STDCALL conventions.

## Disk IO

Disk IO is always 32-bit. Filesystem IO (covered later) can be 16-bit (through DOS) or 32-bit (using a filesystem driver), but the interface is the same.

### KDM_PhysDiskInf(byte disk_num, DiskInfStruct *inf)

### KDM_PhysReadBlocks(byte disk_num, word count, void *copyto)

Read physical blocks from a device. The block size is determined by the DIS structure.

## Debugging

### KDM_Logf(const char *str, ...)
Standard printf. Supports string, char, int, uint.


