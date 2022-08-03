# Introduction

The most powerful feature of OS/90 is the driver model. It is designed to be used for programming devices, buses, and anything requiring ring zero access to the system.

The driver architceture allows bus driver to manage interrupts and other resosurces through the kernel. Device drivers can then communicate with the bus driver to control individual devices and recieve interrupts.

# Definitions

Interrupt: A signal from an external device is typically called an interrupt in this document and elsewhere

Trap: A software interrupt, generated with the INT imm8 instruction (or INTO/INT3)

# What is a Driver in OS/90

A driver is a PE COFF object file with:
* 4K aligned sections
* At least a .text
* An entry point to the message handler
* The file extention .DRV

Drivers are loaded flat into the kernel space after relocation. Because they are loaded at the very begining of startup.

# The Job of Drivers

A driver usually implements the intended function of a certain device, real or virtual, and allows other parts of the system to access it. OS/90 is a hybrid 32/16-bit system. Because of this, it needs a uniform interface so that it decides which components should be used. This is analogous to Windows VxD drivers.

For example, the kernel will access the filesystem through DOS, but a 32-bit FS driver can handle the interrupts itself.

The driver model is what makes OS/90 a true operating system, rather than a protected mode extention to DOS like EMM386.

## Kernel Function Calls

There is a range of functions provided by the kernel that drivers can use.

### Trap Capturing

Summary of defines:
```
CAPT_NOHND = 0 // Driver does not know how to handle thisvector.function
CAPT_HND   = 1 // Driver handled the function successfully.
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

CAPT_STATUS Int21_02(PTrapFrame t)
{
    // DOS putchar: beware, EAX[31:16] could be anything
    if ((byte)(t->regs.eax >> 16) == 2)
    {
        PUTCHAR((byte)t->reg.eax);
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

#### ISA without Plug-and-Play

Old AT-ISA computer buses do not support IRQ/IO steering and are not programmable. PnP should be disabled in this case and bus drivers should not operate.

ISA and PnP-ISA cards can be mixed together in a system, but the resources of the old ISA card must be reserved so the PnP does not use them. The bus device should expect that this is properly configured.

# Mailboxes

The mailbox system is used for driver-driver and kernel-user communication. It is used to implement hotplugging support for buses that allow it. Mailboxes are sort of like the DOS driver interrupt routine.

Mbx is a very low-level interface and it is expected that the driver or user-mode code using it implement proper abstractions to make programming easier. Mbx is used to simplify kernel development.

The following functions are used for mailboxes:
* KeSendMsg
* KeSendRealTimeMsg
* KeRegisterMbx
* KeClearMbx

The mailbox is self-contained and no aditional structures manage it. The kernel facillitates communication, so it memorizes the name of the box.

The maximum number of mailboxes is limited to the maximum number of drivers minus one because the kernel needs a Mbx. The kernel Mbx is takes one message at a time. This is because drivers are initialized in order so there is no benefit.

## Implementation Details

KeSendMsg is in the kernel. The kernel then writes the pointed parameter to the destination mailbox and calls the message handler of the reciever if the queue is filled. The message handler is a cdecl function with a pointer argument to the message structure that was sent. It must check the command number and perform a specific operation. When the operation is complete, a message signaling completion is sent to the caller mailbox. At the end of all this, the initial calling routine will be re-entered and the process completed.

Mailboxes and messages must be DRVMUT. Pointer parameters involving mailboxes are also DRVMUT. This is done because the information is changed by the kernel and other programs. DRVMUT must explicitly declared.

The mailbox structure can be static.

Mbx handling is always performed in a critical section.

There is potential for some small overhead in this system. When performance is critical, messages can be avoided by adding functions to the kernel symbol table. Userspace drivers, however, have to use mailboxes.

## Message Queueing

A mailbox can be opened at any size specified by the requester using KeRegisterMbx. When a command is sent while the mailbox is full, the kernel will call the message handler of the driver or user program. The exact number of messages a Mbx can handle is unimportant to the sender.

The advantage of message queueing is that it gives drivers control over when they want to handle interrupts and other service calls, and in what order.

The kernel, upon initialization of the driver, will send the INIT message and the device driver handles it immediately.

## Standard Commands

Each command is in a block of 32 numbers. This makes it easier to check if a certain class of command have been called using bitwise operations.

### BUS: REQUEST_DEVICE

This message will tell the busdrv that the devdrv wants to control interrupts from this device and recieve a message when they occur. This message does not need to be the result of a real interrupt. No other driver can claim the interrupt line.

### BUS: DEV_NOT_FOUND

### NO_DMA_CTL

This bus driver does not have the ability to change DMA requests.

### ERR_NOT_A_BUS_DRIVER

This driver is not a bus driver and should not have been requested any bus services.

### ERR_NOT_BUS_MANAGED

This device driver is not part of a bus and has not requested a device.

## ERR_UNAUTHORIZED

A user program is not allowed to call this function, only for other drivers.
