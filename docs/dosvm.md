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

For the HMA to be properly emulated, 64K extra must be allocated and mapped to the 16 pages above 1M.
