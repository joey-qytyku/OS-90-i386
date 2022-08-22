# Introduction

The most powerful feature of OS/90 is the driver model. It is designed to be used for programming devices, buses, and anything requiring ring zero access to the system.

The driver architceture allows bus driver to manage interrupts and other resosurces through the kernel. Device drivers can then communicate with the bus driver to control individual devices and recieve interrupts.

# Definitions

Interrupt: A signal from an external device is typically called an interrupt in this document and elsewhere

Trap: A software interrupt, generated with the INT imm8 instruction (or INTO/INT3)

RECL_16: An IRQ reserved for a DOS driver, usually for non-PnP devices, detected by GRABIRQ.SYS. RECL_16 can be explicitly taken over by a 32-bit driver, so that a device works in both environments.

# What is a Driver in OS/90

A driver is a PE COFF object file with:
* 4K aligned sections
* At least a .text, .data, and .bss section
* The file extention .DRV

Drivers are loaded flat into the kernel space after relocation. Because they are loaded at the very begining of startup.

# The Job of Drivers

A driver usually implements the intended function of a certain device, real or virtual, and allows other parts of the system to access it. OS/90 is a hybrid 32/16-bit system. Because of this, it needs a uniform interface so that it decides which components should be used. This is analogous to Windows 9x VxD drivers.

Drivers run only when the kernel decides they should.

For example, the kernel will access the filesystem through DOS, but a 32-bit FS driver can handle the interrupts itself.

The driver model is what makes OS/90 a true operating system, rather than a protected mode extention to DOS.

# Plug-and-Play Support

Plug-and-play is a key feature of OS/90.

PnP is defined as such:
On system startup, after the kernel has finished initializing,
* Devices are disabled, in other words
  * No interrupts, all are masked
  * Resources cannot be disabled at this point, but that is part of the idea
* The PnP BIOS enumerates all devices and lists them in the device file space

Bus drivers act on subordinate devices as the kernel acts on all devices globally:
* Subordinate devices are disabled
  * Resources are not assigned or reconfigured outside the BIOS defaults
  * Interrupts are disabled in the per-device configuration
* If the bus is connected to another one, it may send interrupts after init
* All devices on the bus are to be enumerated and reported in the devfs

Resources are always owned by a bus and lent to other devices. Subordinate device/bus drivers cannot access resources they are not permitted to use by the parent bus.

The kernel is technically a bus driver that controls all system resources. If an independent device driver (LPT, COM, PS/2 mouse) takes an interrupt, it will call to the "kernel bus". This is to keep consistency.

# Resource Management

The kernel has a flat list for IO ports and memory, IRQ lines, and DMA channels. As stated previously, these are only owned by a bus.

System device nodes from the PnP BIOS report resources with the PnP ISA format.

The owner is identified with a pointer to BUSDRV_INFO. This structure contains the name of the device, as well as a unique identification. It also contains information about the segment it belongs to.

## Bus Segmentation

A bus segment is a range of IO ports, memory, DMA channels, and IRQs that are lent to a subordinate bus.

## Device Abstraction

All PnP devices are reported through the devfs. Non-PnP devices simply have their assumed resources blacklisted so that they are not used.

Some devices are embedded to the system board, while others are attached to a bus (PCI, PCMCIA, ISA PnP). Some buses have their own DMA subsystems, while ISA PnP uses the standard AT DMA controller.

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

If the kernel reaches the end of the chain with no handler returning successfully, it will call the real mode version.

The overhead with this feature would be similar to that of a 16-bit DOS function dispatcher, but with the added weight of the CALL instructions and entering through V86 mode to service the interrupt. Basically, this will a bit slower that real mode DOS, but it depends on the number of links.

To add a new capture, a function returning dword and taking a trap frame with external linkage must be written. A TrapCaptureLink variable should point to that variable, other fields are reserved and need not be set Chaining something that already has a handler cannot be prevented, but it is possible to check if it has been handled before.

The following is a functional, though not very useful example. Replacing DOS functions with 32-bit implementations is a key feature and is simple to do.

```c
typedef struct {
    Trap32Hnd handler;
    pvoid     next;
}TrapCaptureLink,*PTrapCaptureLink;

TrapCaptureLink link = {Int21_02, NULL};

Status Int21_02(PTRAP_FRAME t)
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
