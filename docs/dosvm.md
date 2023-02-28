# 16-bit tasks versus Kernel Interrupt Calls

The kernel can call interrupt requests and INTx vectors using special functions. The kernel does not do this for 16-bit tasks. A task running in V86 mode does not require any function to enter. Loading the context is sufficient, which includes the EFLAGS register with the VM bit on. This distinction is important.

In both cases, the monitor is used for handling GPF exceptions to emulate ring-0 instructions.

# TSS

The task state segment contains two important fields, ESP0 and ES0. ES0 does not need to change but ESP0 does. The kernel allocates a stack for each program running on the system, both 16-bit and 32-bit. EnterV86 does not re-enter the caller and simply resumes execution in V86 mode. Only an interrupt or exception can stop the execution of any ring-3 code, as well as V86. When GPF is called from V86, the handler is called and ESP0 is loaded from the TSS. Interrupts and exceptions must work in V86 mode or getting out is impossible, but if ESP0 stays the same and the stack is reset upon each supervisor call, the stack of the caller is destroyed and the system will crash. To prevent this, EnterV86 saves ESP to the TSS.

Task switching never happens when the kernel or drivers are running. Interrupts are enabled and time is updated. This is especially critical for BIOS/DOS calls from protected mode because a task switch will change the kernel stack being used.

# Interrupt Emulation

The INT instruction is emulated by the V86 monitor. Capture chains are used to search for a proper handler for the specific function call. When INT is called, execution is passed to the CS:IP in the physical 1M.

The interrupt chain is global. A kernel-mode virtual trap or an INT call from a DOS program

IRET is a termination code for an ISR and a regular 16-bit V86 program. This is because normal software has no reason to use IRET, and it is impossible to tell when an ISR is trying to exit except by detecting the IRET.

The stack is not modified by virtual IRET and INT because it does not need to be and these instructions have special significance to only the monitor. This means that the monitor is incompatible with an ISR that uses the saved values. There is probably no reason for this anyway.

DOS programs cannot have special hooks for interrupts. They all share the same capture chain.

# Pentium VME

VME allows a special TSS entry for deciding which interrupts are sent to V86 using the IVT or if the GDT is used. It may be supported in the future, but processes will never be allowed to have their own IVTs.

# How a DOS VM is Created

The kernel runs 16-bit threads in V86 mode and captures interrupts if necessary. Setting one up is somewhat complicated and warrants abstraction.

First of all, a 16-bit thread must be created. Its pages must be manually mapped to 1M. Memory allocation must be handled by the virtualization server so that the kernel handles it. Allocation is probably as simple as resizing the thread memory.

The high memory area is somewhat problematic. Nearly all programs that can use it provide an alternative, but this is not enough for full compatibility.

For the HMA to be properly emulated, 64K extra must be allocated and mapped to the 16 pages above 1M, with 16 bytes left usused.

# A20 Gate

The A20 gate is assumed to be on. It is enabled upon boot. Any software that relies on the 8086 address wrap feature will not work on OS/90. I will never add support for enabling or disabling the A20 gate at a software level. Don't ask me to.

On old windows, there is WINA20.VXD which allows the A20 gate to be enabled and disabled at a per-process basis. This could be possible to implement, but as stated previously, I don't care about the A20 gate.

# Direct Hardware Access

Some DOS programs need to access IO ports directly. This would be an error for any other program. It will be impossible to achieve total compatibility, but the goal of OS/90 is to do as good as possible.

## Examples

A DOS game may want to access the keyboard directly and even set its own interrupt handler.

## The DOS Server

The DOS server is a kernel-mode subsystem that handles interrupt reflection to processes using the existing toolset of the kernel API. It is part of the kernel source code.

It handles access to hardware through IO ports and memory mapped IO.

## Virtual Devices

A device driver should register a virtual device with the DOS server if it intends to make the device available to virtual DOS programs. For example, the 32-bit 8042 keyboard driver can allow DOS programs to hook a fake IRQ and access emulated IO ports.

Virtual devices have a simple structure.

```c
typedef struct {
    PIMUSTR     name;
    WORD        irq_bmp;
    DEV_EVHND   dev_event_handler;
    WORD        io_port_base;
    BYTE        io_port_length;
    PVOID       map_to1, map_to2;

    PVOID       next;
};
```
If MMIO or IO is set to NULL, it is unused.

Some devices need to be global in scope but many must be instantiated for individual processes. The per-process "context" of the emulated device is the responsibility of the driver. The device structure is the part the kernel deals with.

There is a difference between RECL_16 interrupts and fake IRQs. RECL_16 is for non-PnP devices with real mode drivers. BUS_INUSE is used for all fake IRQs. The handler can then fake interrupts.

Fake interrupts can be safely sent while inside an ISR.

## Interrupt Handlers

When a DOS program attemps to modify an interrupt vector, the kernel will have to decide how it will respond. This is where per-process virtual interrupts are necessary.

If the interrupt is lines up with where the 8259 PIC would be assigned to the BIOS.

How will this relate to the farcall interface? Will it be a MMIO device?

# DOS Programs and Memory

## Running in Physical DOS

In this situation, the program runs in the actual conventional memory. DOS memory calls are used to allocate the space and write program data. DOS itself cannot handle the loading and execution because it would make multitasking impossible. DOS would try to run the program within a supervisory V86 call.

1MB and 64 KiB are always identity mapped, so this would be simple to do.

This model would reduce the amount of memory available for DOS programs, but is simpler. It may also make DPMI easier to implement.

## Allowing DOS programs to get more memory

This concept could actually work with the other idea. Since one page of the UMA is already in use for far calls, the rest can be used to give DOS programs more memory if they need it. For this to work, the allocation root of the V86 process will have to be initialized.

# Emulation Driver

The emulation driver must exist to control 16-bit process memory and capture function calls. It is separated from the kernel.
