# 16-bit tasks versus Kernel Interrupt Calls

The kernel can call interrupt requests and INTx vectors using special functions. The kernel does not do this for 16-bit tasks. A task running in V86 mode does not require any function to enter. Loading the context is sufficient, which includes the EFLAGS register with the VM bit on. This distinction is important.

In both cases, the monitor is used for handling GPF exceptions to emulate ring-0 instructions.

# TSS and V86 Context Switching

The task state segment contains two important fields, ESP0 and SS0. SS0 will always be set to the flat model segment of the kernel for the switch back to 32-bit mode. The value of ESP is simply what EnterV86 started with before running in protected mode. ShootdownV86 is the other function that V86 uses. When it is called, execution transfers back to the caller of EnterV86. This is normally only called in exception handlers.

If the EFLAGS on the stack has the VM bit enabled, IRET will pop the data segment registers from the stack. This allows us to set the segment registers before entry.

When the V86 program is interrupted, the stack is set to ES0:ESP0 in the TSS and all the segment registers are pushed along with the usual IRET stack frame. The 32-bit segment selectors have to be restored. According to the intel documentation, they are "zeroed" (aka refference null segment). The stack segment is the same as the data segment, and CS:EIP is already set from the interrupt descriptor. This means that restoring the proper register context should be as simple as SS=>DS,ES,FS,GS.

EnterV86 continues execution in V86. Because this function is re-entrant, the stack pointer in the TSS saved must point to the register dump.

See this for more info: https://stackoverflow.com/questions/54845547/problem-switching-to-v8086-mode-from-32-bit-protected-mode-by-setting-eflags-vm

The kernel allocates a stack for each program running on the system, both 16-bit and 32-bit. EnterV86 does not re-enter the caller and simply resumes execution in V86 mode. Only an interrupt or exception can stop the execution of any ring-3 code, as well as V86. When GPF is called from V86, the handler is called and ESP0 is loaded from the TSS. Interrupts and exceptions must work in V86 mode or getting out is impossible, but if ESP0 stays the same and the stack is reset upon each supervisor call, the stack of the caller is destroyed and the system will crash. To prevent this, ScEnterV86 saves ESP to the TSS.

Sometimes the kernel has to access real mode software.

Task switching never happens when the kernel or drivers are running. This is especially critical for BIOS/DOS calls from protected mode because a task switch would change the kernel stack being used.

# Interrupt Reflection and Capturing

The INT instruction is emulated by the V86 monitor. Capture chains are used to search for a proper handler for the specific function call. When INT is called, execution is passed to the CS:IP in the physical 1M.

IRET is a termination code for an ISR and a regular 16-bit V86 program. This is because normal software has no reason to use IRET, and it is impossible to tell when an ISR is trying to exit except by detecting the IRET.

The stack is not modified by virtual IRET and INT because it does not need to be and these instructions have special significance to only the monitor. This means that the monitor is incompatible with an ISR that uses the saved values. There is probably no reason for this anyway.

## The Local Interrupt Problem

The interrupt chain is global. A kernel-mode virtual trap or an INT call from a DOS program will cause the kernel to iterate through it and look for protected mode handler until the request is resolved or it gets reflected to DOS. A problem arises when applications attempt to set/get/hook interrupt vectors, IDT or IVT, using API calls. The program expects to have its ISR called when it runs INT or a fake IRQ is sent. There are many reasons why a program may need to do this. If the capture chain handles a V86 trap for the program that is running, the hook will not be able to run.

The solution is to get the PID of the currently running process and traverse a separate linked list. Each process has a link to the next process. Each process has its own array of interrupt information entries. Each entry decides if the virtual interrupt should be reflected to physical DOS handled by a 32-bit ISR, or be handled by a 16-bit DPMI hook.

```c
// If the program calls INT with this, reflect to DOS
// using the global capture chain
// The default for DOS except for INT 21H and INT 31H
LOCAL_PM_INT_REFLECT_GLOBAL

// This interrupt vector points to an ISR for a fake IRQ
// It should not be called by INT or program crashes
LOCAL_PM_INT_PM_FAKE_IRQ

// This interrupt vector was modified by the program with a PM trap handler
// If INT is called, it is caught by the #GP handler and passed to the DOS
// server for emulation.
LOCAL_INT_PM_TRAP

typedef struct {
}VIRTUAL_PMODE_IDT_ENTRY;
```

Real mode interrupt hooks must be handled separately as they can be called independently of the protected mode vectors using a DPMI call.

# How a DOS VM is Created

The kernel runs 16-bit threads in V86 mode and captures interrupts if necessary. Setting one up is somewhat complicated and warrants abstraction.

First of all, a 16-bit thread must be created. Its pages must be manually mapped to 1M. Memory allocation must be handled by the virtualization server so that the kernel handles it. Allocation is probably as simple as resizing the thread memory.

The high memory area is somewhat problematic. Nearly all programs that can use it provide an alternative, but this is not enough for full compatibility.

For the HMA to be properly emulated, 64K extra must be allocated and mapped to the 16 pages above 1M, with 16 bytes left usused.

# A20 Gate

The A20 gate is assumed to be on. It is enabled upon boot. Any software that relies on the 8086 address wrap feature will not work on OS/90. I will never add support for enabling or disabling the A20 gate at a software level. Don't ask me to. On old windows, there is WINA20.386 which allows the A20 gate to be enabled and disabled at a per-process basis. This could be possible to implement, but as stated previously, I don't care about the A20 gate.

# Execution of DOS Programs

All real mode software runs in physical DOS. Addresses in 16-bit mode are identity mapped. This approach reduces the available memory when multitasking. This also requires that a real mode stack are allocated by the DPMI client before initialization.

Environments are easily accessible this way.

## Executing a Process

Loading programs must be done by the 32-bit kernel rather than DOS because DOS will immediatetely execute the program until it returns. The function for executing a new process is trapped.

Function 4Bh can execute a program immediately or load an overlay. When a program exits, the exit status of the subprogram is retrieved. This can be implemented inside the process control block. The exit function (4Ch) is trapped and will kill the subprocess.

Creating a subprocess suspends the parent process until completion because DOS software is not designed to be multithreaded.

Loading executables is simple. MZ files contain a list of fixups. They indicate addresses that need updating to be segment-relative. The program CS is added to the specified addresses so that they are relative to the base address of the executable image. COM files are just flat binaries with addresses slanted 256 bytes forward for the PSP. Loading the files involves opening the executable and reading its contents into a block of conventional memory. EXE files have fixups applied and execution can begin.

If a DPMI program runs EXEC, the subprocess will run as usual, but in real mode.

## XMS

XMS uses 16-bit handles and memory freezing. Memory handles 0-65535 are garaunteed to be XMS reserved.

Some drivers may use XMS. When the kernel initializes, it will run the OS inside the largest possible XMS block, leaving all others blocks intact.

What if DOS is called by the kernel through a supervisory call? If such a program freezes an XMS block, how should this be handled? The XMS API must be virtualized because XMS will switch to protected mode in order to copy between extended and conventional memory. Should the previously allocated handles be probed by the kernel?

## Expanded Memory

# Direct Hardware Access

Some programs need to access IO ports directly.

## Examples

A DOS game may want to access the keyboard directly and even set its own interrupt handler.

## The DOS Server

The DOS server is a kernel-mode subsystem that handles interrupt reflection to processes using the existing toolset of the kernel API. It is part of the kernel source code. It handles access to hardware through IO ports and memory mapped IO.

It does not provide any virtualization for 16-bit legacy IRQ handlers. The point is to provide hardware emulation when necessary for 16-bit V86 processes.

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
The IRQ bitmap specified which interrupt hooks the driver must respond to.

The device IO request packet is passed to the event handler.
IO_CODE is a bitmap which can be one of the following or'ed together.

* DEVIO_PORT
* DEVIO_MEM
The size is specified using only one of the following:
* DEVIO_ACC8
* DEVIO_ACC16
* DEVIO_ACC32
E.g. DEVIO_PORT | DEVIO_ACC16

```c
typedef struct {
    IO_CODE     type;
    DWORD       address;
    DWORD       pid;
}DEVIO_REQ_PACKET;
```

If MMIO or IO is set to NULL, it is unused.

Some devices need to be global in scope but many must be instantiated for individual processes. The per-process "context", which can include other data (e.g. the PID), of the emulated device is the responsibility of the driver. The device structure is the part the kernel deals with. The vector data structure is ideal for managing the contexts.

There is a difference between RECL_16 interrupts and fake IRQs. RECL_16 is for non-PnP devices with real mode drivers. BUS_INUSE is used for all fake IRQs. The handler can then fake interrupts. Fake interrupts can be safely sent while inside an ISR.

## Interrupt Handlers

When a DOS program attemps to modify an interrupt vector, the kernel will have to decide how it will respond. This is where per-process virtual interrupts are necessary.

If the interrupt lines up with where the 8259 PIC would be assigned to the BIOS, the driver is signaled through the standard event notification routine of the attempted hook.

DOS programs hooking other vectors is not possible.

How will this relate to the farcall interface? Will it be a MMIO device?

## Post hooking and Pre-Hooking

It may be necessary to monitor or modify the input or output of a DOS call rather than replace it completely. Internally, the handler can either be 32-bit or 16-bit. A 32-bit handler can manually enter V86 for both post hooks and pre-hooks.

# DPMI

This may be the most important section in the documentation. DPMI inflences many of the kernel design choices. DPMI is a standard that provides a very low-level interface to DOS programs that is difficult, but not impossible, to virtualize.

DPMI does not specify an executable format. Loading executable data is handled by a real-mode stub.

The implementation used by OS/90 is described in the specification as a fully virtualized environment. OS/90 is not DOS and emulates or arbitrates all devices accessed by a DOS program. DPMI programs are virtualized DOS applications.

DPMI version 0.9 is the most widely used version and is implemented in OS/90.

## Memory

DPMI also has support for demand paging and marking pages as candidates. This is unnecessary because OS/90 will automatically handle memory management, so these services are ignored.

## Interrupts

### Interrupt Reflection Defaults

DPMI mandates that software interrupts in the protected mode environment are reflected to their 16-bit DOS counterpart by default, with INT 31H and INT 21H AH=4Ch being exceptions. That means the DOS interface will basically operate exactly the same except in protected mode.

How would IRQs work then if we HAVE to remap the PIC in order to avoid conflict with the exceptions? The answer is simple.

* The IDT allows specifying the ring that the IDT entry can be called from using INT
* All interrupt vectors are ring 0.

This means that a user program that calls an IRQ or system exception vector will generate a protection fault, allowing the INT instruction to be emulated, and the interrupt reflection using V86 can take place.

Each process has data for every interrupt entry. Each entry indicates if the vector is to be reflected or if it has a protected mode handler assigned to it. If it has a protected mode handler, it is called.

Changing vector 31H is not allowed and is a critical error.

### Interrupt Hooking

DPMI provides API calls for modifying real mode and protected mode interrupts. This is impossible to do directly. Both protected mode and real mode interrupts are faked, along with the state of IF.

There can only be one handler per IDT entry. Each DPMI client must have its own virtual IDT. The problem is with the INT instruction being able to call them directly. This will not happen. All interrupt vectors will be ring zero and will be reflected to DOS if it was never modified or called. Interrupt vectors which would normally be IRQ vectors can be hooked by protected mode or real mode DOS applications, both invoking interrupt faking.

The kernel has a global interrupt capture chain. It applies to drivers and the kernel when calling V86 interrupts. DPMI applications can change real mode and protected mode vectors.

#### Getting and Setting

DOS and DPMI provide functions for changing and getting addresses of interrupt vectors. This is used to implement hooking without losing the current handler. Both these functions must be virtualized.

### Allocating DOS Memory

This is possible and necessary for the kernel, though DPMI 0.9 does not require this to be exposed to the API.

Never run allocation procedures in an ISR unless in a critical section.

### Local Descriptor Table

Services for modifying the local descripor table are provided by DPMI. It could be possible to stuff all DPMI LDT entries in the same system-wide LDT, but this would be a limitation on the number of processes that can be running. Programs will need at least a data and code descriptor. That would mean 4096 processes at once. This would not be a problem, and the LDT services can be virtualized. It is wasteful to allocate memory for separate processes to have truly local LDTs.

This makes shared memory easy. Processes can access shared segment selectors. GCC has support for FS and GS relative pointers.

The following functions are implemented by DPMI for the LDT:

Allocate LDT descriptors: A range of selectors is allocated and the base selector is returned.
Free LDT descriptor
Segment to descriptor: Returns a selector that points to a real mode segment. Limit is 64K

Set selector base address: Most programs will set them to zero for the flat model and because GCC does not support segments

Set segment limit: Most programs will set this to FFFFFFFF.

Set descriptor access rights: Ring three segment seem to be required when calling this function, or equal to caller CPL. 32-bit code and data segments with page granularity are used.

Set descriptor: This sets the value of the descriptor. The descriptor is parsed by the kernel because allowing direct access to the LDT would not work.

Query Descriptor: This gets the value of a descriptor

Selector Increment Value: I am not sure why this exists. I guess it is just the value added to get the next descriptor in the array. There are no arguments for it.
The value returned is:  1<<4 so that it does not add into the RPL and LDT/GDT bits.

Allocate specific LDT descriptor: I wonder how many programs use this feature and why. It allocates a specific descriptor to the caller. This is not implemented for now. There really is no purpose for most programs.

### Memory Allocation and Unified MMGR

The specification of the memory manager defines a similar interface to DPMI. Both use a handle-based memory allocation system. DPMI returns the address of the block immediately after allocating, so the kernel freezes the block as soon as it is created. LDT entries must be set to point to the newly allocated block.

For the flat model, the LDT entries point to zero. The address returned by this function is a virtual address to the data. Segments are merely offsets to the linear address. This is used for loading the executable data into usable memory. For this reason, userspace software must be linked to use addresses above 1M+64K.

OS/90 has a unified memory management interface. XMS, DPMI, and the userspace API refer to the same internal functionality. XMS handles are a subset of the DPMI handles, as mentioned previously. This means that freeing an XMS block using DPMI is theoretically possible, though not recommended.

Userspace software designed for OS/90 should use the dedicated memory API for proper heap managment. This way, memory does not need to be auto-frozen.

DPMI 1.0, which is not implemented in OS/90, specifies an interface for allocating and freeing DOS memory blocks in the conventional memory space. The kernel implements this feature, but DPMI does not have this because of 0.9 compliance.

#### INT 31H 0800H: Physical Address Mapping

This maps a physical address to a virtual address. The memory manager provides this functionality, although it is not virtual at all. To make this work, it must first go through the MMIO of any devices using the memory, so the virtual address space has to be mapped to an appropriate location for the IO. There is little reason for a DPMI application to access any other memory using this function, so access to any other memory is forbidden and restricted to only device virtual IO. This function is byte granular. I will just implement it as page granular and hope it works. Usually, this function is used to access video memory or some other memory-mapped IO region of a physical device, or in this case, virtual.

## 16-bit Procedure Calling

All virtual far calls are implemented by allocating a far call entry. An unmapped section of memory in the BIOS ROM space grabs the CS:EIP of the error and indexes a far call handler. Far call allocations are GLOBAL in scope and shared by all programs. This is not recommended as a substitute for shared libraries. Only 16 far calls are available.

DPMI software can also call 16-bit code using interrupt and far call stack frames. These functions allow for copying data from the protected mode stack. The farcall mechanism is used to implement far returning. The address to which the real mode address returns to is in the special memory area.

## Entering and Exiting Protected Mode

All DPMI programs start in real mode. They must obtain a far call address to enter protected mode. An INT 2FH service is used for obtaining an entry point. When the entry point address is FAR CALL'ed, the program starts running in protected mode right after the FAR CALL instruction. This is used to implement XMS and the entry to protected mode routine.

The get entry point function requests to extra conventional memory (why would it need it?). All segments selectors are set to point to what the used to point to in protected mode, except for ES, which points to the PSP.

When a program is done running in protected mode, it can call INT 21H AH=4CH.

Then it will terminate. This does terminate the program that started the DPMI session.
