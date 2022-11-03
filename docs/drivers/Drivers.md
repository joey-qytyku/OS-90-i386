# Introduction

The most powerful feature of OS/90 is the driver model. It is designed to be used for programming devices, buses (PCI, ISA, VLB, etc.), and anything requiring ring zero access to the system.

The driver architceture allows bus driver to manage interrupts and other resosurces through the kernel. Device drivers can then communicate with the bus driver to control individual devices and recieve interrupts and events.

DM90 is intended to be somewhat portable to other operating systems. It is also intended to support the limitations of 90's hardware.

# Definitions

Interrupt: A signal from an external device is typically called an interrupt in this document and elsewhere unless specified otherwise.

Trap: A software interrupt, generated with the INT imm8 instruction (or INTO/INT3)

# What is a Driver in OS/90

A driver is a PE COFF object file with:
* 4K aligned sections
* At least a .text, .data, and .bss section
* The file extention .DRV

Drivers are loaded flat into the kernel space after relocation. Drivers can be inserted and removed at any time, or at least they will be in the future.

# The Job of Drivers

A driver usually implements the intended function of a certain device, real or virtual, and allows other parts of the system to access it. OS/90 is a hybrid 32/16-bit system. Because of this, it needs a uniform interface so that it decides which components should be used. This is analogous to Windows 9x VxD drivers.

Drivers run only when the kernel decides they should.

For example, the kernel will access the filesystem through DOS, but a 32-bit FS driver can handle the interrupts itself.

The driver model is what makes OS/90 a true operating system, rather than a protected mode extention to DOS.

# General Notes

* Never assume the position of elements in structures

# Plug-and-Play Support

Plug-and-play is a key feature of OS/90.

PnP is defined as such:
On system startup, after the kernel has finished initializing,
* Devices are disabled, in other words
  * No interrupts, all are masked
  * Resources cannot be disabled at this point, but that is part of the idea
* The PnP BIOS enumerates the mainboard and lists devices in the devfs

Bus drivers act on subordinate devices as the kernel acts on all devices globally:
* Subordinate devices are disabled
  * Resources are not assigned or reconfigured outside the BIOS defaults
  * Interrupts are disabled in the per-device configuration
* If the bus is connected to another one, it may send interrupts after init
* All devices on the bus are to be enumerated and reported in the devfs

Resources are always owned by a bus and lent to other devices. Subordinate device/bus drivers cannot access resources they are not permitted to use by the parent bus.

The kernel is technically a bus driver that controls all system resources. If an independent device driver (LPT, COM, PS/2 mouse) takes an interrupt, it will call to the "kernel bus". This is to keep consistency.

# Resource Management

A resource is a DMA chanel, IO port range, IRQ line, or memory mapped IO location. The kernel has a list for all resources, IRQ lines, and DMA channels.

System device nodes from the PnP BIOS report resources with the PnP ISA format.

The owner is identified with a pointer to BUSDRV_INFO. This structure contains the name of the device, as well as a unique identification. It also contains information about the segment it belongs to.

## PnP BIOS and Motherboard Resources

The PnP resource information is reported as a tag-based structure also used for ISA cards. The IRQ and DMA resource items indicate which lines and channels can be used by the device. If a device has an interrupt mask of FFFF, it can be configured to use any IRQ. Same applies with DMA.

This means that resource usage is indicated only if one of the IRQ or DMA bits is turned on, informing the OS that the device can only use that interrupt if it is to be operational.

Some devices like the PIC or DMA controller are reported through PnP, but they cannot be configured.

System board devices are not automatically configured as there is little reason to do this. A user can manually reconfigure them if desired.

The PnP BIOS does not report everything. It will not report PCI devices. PCI VGA cards are hardcoded to use A0000h-BFFFFh and use BARs for SVGA VRAM.

## Problems

### Limited Configurability

The ISA bus is 16-bit so it does not support 32-bit memory access or addressing. The ISA PnP specification seems to support this anyway. ISA also can use 10-bit port decode or 16-bit decode. The ISA PnP static resource data reports this.

In the ISA bus, each card is sent the pin signals and, hopefully, only one responds by transmitting data. The difference in address decode bits is problematic because a card with 10-bit IO decode will only get 10 bits, and the upper ones mean nothing. E.g. if the base is 2F8, the address AF8 would access the exact same address. This can become a source of conflict.

PCI, by comparison, is 32-bit capable and BARs can go anywhere.

The ISA PnP bus has more specifics that require specific design choices in the PnP manager. Devices can have certain memory addresses and ports which they can optionally be configured to use by software. For example, a parallel port card can be configured to the three standard ports and nothing else for compatibility.

### User Configuration

The user may need to change certain settings. Configurations must be maintained by the userspace.

## Abstract Devices

It is up to the bus driver to decide how the devices are accessed using the API and how they are stored internally, but devices should be reported in a standard format within the DevFS.

Devices should have a variable in their structures (or maybe a bitmap) that indicates that it has been requested so that duplicate devices can work.

### DevFS Device Format

The goal is to expose devices to userspace so that plug-and-play driver loading and configuration utilities can be devloped. DevFS does not need to be used but should be.

The file stores the following information:
* Resource information in ISA PnP format

## Interrupts

### Concept

The 8259 PIC is abstracted by the interrupt subsystem. Instead, a 32-bit virtual interrupt request, or VINT, are assigned to interrupt vectors, and VINT is local to a specific bus. As previously mentioned, the kernel is the low-level bus driver that controls all resources on startup.

The VINT is physical to the kernel bus, so VINT 15 is the same is the actual IRQ 15. The kernel can be modified to support IOAPIC.

### Rules

Interrupt-related routines cannot be within an ISR. This is undefined behavior. 

The kernel modifies interrupt entries within a critical section.

### Types

An interrupt can be:

BUS_FREE:
This interrupt is managed by a bus and cannot be taken by any other driver, except one that does through this specific bus. The utility pointer refferences the bus driver header.

BUS_INUSE:
It is taken by a bus-subordinate driver. IRQ sharing is possible, but only under the terms of the bus driver. The kernel bus does not permit this.

For the kernel, this IRQ was detected by the PnP BIOS and is statically assigned to a plug-and-play system board device. It cannot be taken for use by any driver.

RECL_16:
A reclaimable IRQ which is sent to real mode. These are detected by grabirq.sys, which detects changes to the interrupt vector table. Typically used with DOS drivers for non-PnP hardware. Utility pointer has no meaning.

The utility pointer has a slighly different meaning depending on the interrupt level.

All interrupt ownership requests go through a bus driver, which can be the kernel.

### Interrupt Callbacks

The kernel calls the owner of the IRQ and the owner has to handle it. It decides how it will notify the driver that an interrupt has occured.

### Legacy Support

Static interrupts for legacy ISA cards cannot be detected and are to be specified by the user. If a DOS driver is installed, it will have modified the interrupt vector table to hook/insert an IRQ handler.

RECL_16 is detected by grabirq or specified by the user, and the interrupt is reflected to DOS.

A 32-bit driver for an non-PnP ISA card uses InAcquireLegacyIRQ to change it from BUS_FREE or RECL_16 to BUS_INUSE, with the owner being the kernel.

### API

```
InAcquireLegacyIRQ();
VINT InRequestBusIRQ(VINT);

```
STATUS ScanFreeIRQ(PBUS_DRIVER, IN OUT PWORD iterator);

Scanning IRQs is not done in a critical section, so an iterator must be provided. The iterator is the IRQ index. This value must be saved for sequential calls of the function so that it does not report the same IRQ.

The status is OS_OK if the IRQ was found and OS_ERROR_GENERIC if there are no free interrupts.

It is used like this:

```
WORD interator = 0;
STATUS is_there_free = ScanFreeIRQ(pkernel_bus, &iterator);

if (is_there_free == OS_OK)
    // Found an available IRQ on this bus
if (is_there_free == OS_ERROR_GENERIC)
    // Could not find one, restart loop
```

## Device Abstraction

All PnP devices are reported through the devfs. Non-PnP devices simply have their assumed resources blacklisted so that they are not used.

Some devices are embedded to the system board, while others are attached to a bus (PCI, PCMCIA, ISA PnP). Most buses have their own DMA subsystems, while ISA PnP uses the standard AT DMA controller.

Assuming the presence of devices is avoided, but some hardware is garaunteed to be present, such as the 8259 PIC and DMA. The PnP BIOS is called by the PnP manager to detect system board devices. These devices are never re-configured (although it would be possible). Detected system board devices are placed in the $:\SYSTEM namespace.

# Programming

There is a range of functions provided by the kernel that drivers can use. These are exposed through the kernel symbol tree, a list of absolute addresses with 28-character names and 32-bit addresses, making each entry 32 bytes in size. The symbol table is static and does not change.

The driver model permits dynamic loading and unloading, which could be implemented in the future. A driver should be prepared to handle an unload event.

## Trap Capturing

Summary of defines:
```
CAPT_NOHND = 0 // Driver does not know how to handle this_vector.function
CAPT_HND   = 1 // Driver handled the function successfully.
CAPT_NUKE  = 2 // Nuke the process that called this interrupt
// Eg: CAPT_NUKE | CAPT_HND
```

Capturing traps involves the creation of a chain of handlers. If one cannot handle the 16-bit trap (e.g. not the function code it wants) it is passed down to a driver that may handle it. A structure in the kernel already exists for all 256 real mode interrupt vectors. This structure is passed to the handler. If the handler cannot handle the requested, it must set the return status to CAPT_NOHND and yield to the kernel. The kernel will then call the next one upon realizing that the handler cannot perform the task.

Interrupt requests cannot be captured, but the vector can be "multiplexed", so a software interrupt can have a different meaning than a hardware interrupt.

If the kernel reaches the end of the chain with no handler returning successfully, it will call the real mode version.

Some software interrupts are hooked by DOS software before running the program. This is okay.

The overhead with this feature would be similar to that of a 16-bit DOS function dispatcher, but with the added weight of the CALL instructions and entering through V86 mode to service the interrupt. Basically, this will a bit slower that real mode DOS, but it depends on the number of links. Range checking is the correct way to implement function dispatching.

To add a new capture, a function returning dword and taking a trap frame with external linkage must be written. A TrapCaptureLink variable should point to that variable, other fields are reserved and need not be set Chaining something that already has a handler cannot be prevented, but it is possible to check if it has been handled before.

The following is a functional, though not very useful example. Replacing DOS functions with 32-bit implementations is a key feature and is simple to do.

```c
typedef struct {
    Trap32Hnd handler;
    PVOID     next;
}TRAP_CAPTURE_LINK,*PTRAP_CAPTURE_LINK;

TrapCaptureLink link = {Int21_02, NULL};

STATUS Int21_02(PTRAP_FRAME t)
{
    // DOS putchar: beware, EAX[31:16] could be anything
    if ((byte)(t->regs.eax >> 16) == 2)
    {
        PUTCHAR(t->reg.eax & 0xFF);
        return CAPT_HND;     // Tell the kernel that I handled it
    } else
        return CAPT_NOHND;   // This is not my function
}

void FooBar(void)
{
    ScAppendTrapLink(0x21, &link);
}

```

Trap handlers are only called when DOS calls the INT instruction. IRQs are handled separately, although there is no reason to call an ISR with INT.

### Interrupt Handling

Handling interrupts is possible with the bus/device model of driver communication. A device driver can be subordinate to the bus driver and recieve interrupts through it.

A bus can claim any free interrupts that it needs for IRQ steering. RECL_16 cannot be claimed. STANDARD_32 can be "stolen" by a device driver but never by a bus, so that standard hardware can be implemented in the kernel and in drivers.

Interrupts owned by the bus are signaled by the kernel, and the bus uses callbacks to signal the final handler function.

Device drivers can request a device on a bus by signaling the bus driver with the appropriate ID. The exact format of the ID can be anything that fits in a 256-bit number and must be known by the busdrv and devdrv. A driver can support the same device with different capabilities, such as native mode IDE vs legacy IDE. It can call the request function several times to probe the capabilities, but this should be avoided and the driver should support all possible cases.

Duplicate devices are possible and must be handled by the bus. It does not technically matter which one a device ends up using, but the duplicates should be accounted and reported so that the device driver can control both devices if possible. In the case of PCI, the vendor and device ID would classify a device, rather than the prog-if of capabilities. Example, two PCI-IDE controllers.

The bus driver is supposed to list each device using some kind of internal format. To expose these to userspace, the bus driver can open a mailbox for them.

# Device Files

Device files are accessible through DOS and the 32-bit kernel and userspace uisng the dollar sign as a drive letter. It is used to allow userspace to access devices and kernel-mode features.

The device filesystem is stored in the memory and can have folders and files. Device files do not contain data, they simply "contain" a handler function.

## Implementation

The device FS uses one structure for directories and files. If the type of the structure is a directory, the pointer refferences the start of the file list. If the type is a device file, it points to the next device of directory in the filesystem. If the link is NULL, that is the end of the directory chain.

In order for a directory to point to the next dir/file, separate pointers are used.

```
typedef struct *__PSysDevice{
    byte            name[11];
    byte            type;
    __PSysDevice    next_file,subdir;
}SysDevice,*PSysDevice;
```
## Included devices

```
\PNPBIOS
    \PNPxxxx
\SHUTDOWN
\BLOCK
    \IDEx
        \PARTx
    \FDA
    \FDB

```
