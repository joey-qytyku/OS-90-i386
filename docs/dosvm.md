# 16-bit tasks versus Kernel Interrupt Calls

The kernel can call interrupt requests and INTx vectors using special functions. The kernel does not do this for 16-bit tasks. A task running in V86 mode does not require any function to enter. Loading the context is sufficient, which includes the EFLAGS register with the VM bit on. This distinction is important.

In both cases, the monitor is used for handling GPF exceptions to emulate ring-0 instructions.

# TSS

The task state segment contains two important fields, ESP0 and ES0. ES0 does not need to change but ESP0 does. The kernel allocates a stack for each program running on the system, both 16-bit and 32-bit. EnterV86 does not re-enter the caller and simply resumes execution in V86 mode. Only an interrupt or exception can stop the execution of any ring-3 code, as well as V86. When GPF is called from V86, the handler is called and ESP0 is loaded from the TSS. Interrupts and exceptions must work in V86 mode or getting out is impossible, but if ESP0 stays the same and the stack is reset upon each supervisor call, the stack of the caller is destroyed and the system will crash. To prevent this, EnterV86 saves ESP to the TSS.

Task switching never happens when the kernel or drivers are running. Interrupts are enabled and time is updated. This is especially critical for BIOS/DOS calls from protected mode because a task switch will change the kernel stack being used.

# 16-bit tasks

