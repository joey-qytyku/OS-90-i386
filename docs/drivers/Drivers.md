# Introduction

The most powerful feature of OS/90 is the driver model. It is designed to be used for programming devices, buses, and anything requiring ring zero access to the system.

The driver architceture allows bus driver to manage interrupts and other resosurces through the kernel. Device drivers can then communicate with the bus driver to control individual devices and recieve interrupts.

# What is a Driver in OS/90

A driver is a PE COFF object file with:
* 4K aligned sections
* At least a .text
* An entry point to the message handler
* The file extention .DRV

Drivers are loaded flat into the kernel space after relocation. Because they are loaded at the very begining of startup.

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

## Interrupts

Interrupt requests can be fast or normal. A fast interrupt is directly called and IF=0 on entry. A normal interrupt is signaled with a message and other interrupt are allowed. Soft interrupt have the advantage of being easier to control in software. For example, if the handler detects several interrupts in the queue, it can repeat the same operation, and other device-specific optimizations. This is not useful for interrupts that occur frequently.

Messages in the queue always appear in the order that they are issued.

Message handling is always done within a critical section.

A dusdrv must claim a set of interrupt lines that are marked FREE and not RECL_16. A direct function call exists for this.

## Standard Commands

Each command is in a block of 32 numbers. This makes it easier to check if a certain class of command have been called using bitwise operations.

### BUS: REQUEST_DEVICE

This message will tell the busdrv that the devdrv wants to control interrupts from this device and recieve a message when they occur. This message does not need to be the result of a real interrupt. No other driver can claim the interrupt line.

### BUS: DEV_NOT_FOUND
### HANDLE_IRQ

For buses like ISA PnP, finished DMA requests are signaled through IRQs. HANDLE_IRQ can also signal the end of a DMA operation.

### NO_DMA_CTL

This bus driver does not have the ability to change DMA requests.

### ERR_NOT_A_BUS_DRIVER

This driver is not a bus driver and should not have been requested any bus services.

### ERR_NOT_BUS_MANAGED

This device driver is not part of a bus and has not requested a device.

## ERR_UNAUTHORIZED

A user program is not allowed to call this function, only for other drivers.
