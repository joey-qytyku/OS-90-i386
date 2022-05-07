# DOS Virtual Machines (VM16)

DOS virtual machines can be created by the operating system and other supervisory components to emulate real mode programs with different options. Direct hardware,  memory, and BIOS access can be configured by the requesting module. Tasking can also be configured to be exclusive or concurrent and video memory can be directly accessed or mapped to a virtual framebuffer. Only standard VGA modes support this feature.

The goal

The VM16 interface is for running 16-bit programs and drivers in the same namespace as native 32-bit processes (VM32).

# Abstract

DOS virtual machines are processes that run in VM86 mode. They do not have access to kernel API functions and are isolated from the system.

# DOS emulation

The DOS kernel is not emulated, but DOS functions are redirected to the kernel in the physical conventional memory through the real mode IVT, which is mapped into the virtual VM86 address space. All configurations of a virtual machine can access the DOS system. Certain features like allocating memory are controlled by kernl386 because DOS will simply use conventional memory, which is not available for the VM.

## Forking and Closing Multiple

Processes may need to run in a single virtual machine?

There is an exec function in DOS and it involves memory allocation. Because allocs are not transparent to the. This function cannot go through DOS kernel. Every time a new program runs, a new VM is created with the same configuration as the last.
Programs

The DOS kernel implements a function called EXEC which runs a program by name. This function is trapped. The last programs size is stored by the kernel so that the next one is loaded after in the same virtual machine.

## Environment

It is stored where the BIOS rom space would normally be. The environments

# VM86 Monitor Implementation (Internal)

The VM86 monitor is the low-level interface for 16-bit virtual machine management. It is abstracted by higher level scheduler functions. It is responsible for simply entering VM86 mode, getting out, and emulating privileged instructions if a variable is set. This is to minimize linking of C and assembly.

The function VM86CritSec is implemented in assembly and is callable from C using cdecl. This function disables interrupts, loads a new TSS with a 16-bit stack (ESP3) and a 32-bit supervisor stack, and re-enables interrupts. Interrupts will go directly to their respective IDT vectors. IRQ ISRs call a subroutine that checks the in service register of the 8259A PICs (both) and if it matches with a redirection entry, the interrupt is ricocheted to the 16-bit interrupt vector table.

The redirection entry applies to traps as well. Exception vectors should not be called by DOS programs, but if they are, the VM will simply terminate.

The internal asm function SimulateInt16 is used to simulate real mode interrupts and must run in a VM86CritSec. If only an interrupt must be simulated, the monitor, which is written in asm, can call it.

A redirection map (unrelated to virtual mode extentions)

byte vm86_pl; // USER or KERNEL
byte redir_map[32];

VM86Enter(TSS *tss, dword esp3, dword cs_ip)
VM86Exit();

## Future Improvements

VME support could be added so that 16-bit ISRs can run directly.

# Not all programs will work

It is inevitable that some DOS programs will not run in the virtual machine environment.

# Memory

A VM16 must be provided memory by the control program using the API_PAlloc function. A program can be loaded with a separate function.

If the framebuffer needs to be simulated in separate memory (for text mode it likely is) or as a hack to get double buffering,

# API

CreateVM16(struct VMControl vmctl)

## VM Control block

```c++
struct VMControl
{
	byte conf_bytes[16];
	word memblock4k;
};
```

### Memblock4k and VM RAM

This is the amount of memory to allocate initially.

### 0: Task mode

0=Exclusive
1=Concurrent

An exclusive VM halts the scheduler's normal operation and only runs one program.

### 1: Video configuration

|F|V|

V: Use video (if 0 rest are unused)
F: Change mode and enter fullscreen

If F=0,
If a video mode is changed using INT 10H, the interrupt is trapped and the mode is forcefully switched. VGA registers are permitted to be used by changing the IOPB in the VM86 TSS.

### 2: BIOS, hardware, etc. directly accessible

IN and OUT instructions are emulated. BIOS calls are accessible. Video can be used, so fullscreening to DOS is possible. If bit 7 is set, exclusive tasking is implied. Note that all BIOS interrupts block the scheduler and are done with interrupts disabled. An attempt to enable or disable interrupts will simply do nothing

## 3-35 Software: Interrupt capture table SICT

Used to trap __software__ interrupts and redirect them to a software handler. If a bit is zero, the interrupt is 32-bit and serviced by the IDT. If it is 1, it is redirected to DOS.

32-bit traps are serviced from a call table.

Any interrupt can be captured by any 32-bit supervisor except for those used by the BIOS and DOS. Attempting to hook any of these will cause a configuration error. 

## VMC Examples

DOS programs are run by a system service that traps DOS API calls and redirects some of them to the kernel.

```
void VM16test(void)
{
	VMControl vmc {
		vmc.conf_bytes = {0x1, 0x2, 0};
	}
	API_CreateVM16(&vmc);
}
```

# Call 5

This system call interface was introduced with MS-DOS for portability with CPM, which used a similar interface. There are no plans to support this feature in the kernel DOS program loader. Just use regular DOS for software that old.
