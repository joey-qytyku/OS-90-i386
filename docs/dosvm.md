# DOS Virtual Machines (VM16)

DOS virtual machines can be created by the operating system and other supervisory components to emulate real mode programs with different options. Direct hardware,  memory, and BIOS access can be configured by the requesting module. Tasking can also be configured to be exclusive or concurrent and video memory can be directly accessed or mapped to a virtual framebuffer. Only standard VGA mode support this feature.

The VM16 interface is for running 16-bit programs and drivers in the same namespace as native 32-bit processes (VM32).

# Abstract

DOS virtual machines are processes that run in VM86 mode. They do not have access to kernel API functions and are isolated from the system.

# DOS emulation

The DOS kernel is not emulated, but DOS functions are redirected to the kernel in the physical conventional memory through the real mode IVT, which is mapped into the virtual VM86 address space. All configurations of a virtual machine can access the DOS system. Certain features like allocating memory are controlled by kernel386 because DOS will simply use conventional memory, which is not available for the VM. Forking programs creates a new virual machine with the exact same configuration.

# VM86 Monitor Implementation (Internal)

The VM86 monitor is the low-level interface for 16-bit virtual machine management. It is abstracted by higher level scheduler functions.

The function VM86CritSec is implemented in assembly and is callable from C using cdecl. This function disables interrupts, loads a new TSS using a 16-bit stack (ESP3) and a 32-bit supervisor stack, and re-enables interrupts. Interrupts will go directly to their respective IDT vectors. IRQ ISRs call a subroutine that checks the in service register of the 8259A PICs (both) and if it matches with a redirection entry, the interrupt is ricocheted to the 16-bit interrupt vector table.

The redirection entry applys to traps as well. Exception vectors should not be called by DOS programs, but if they are, the VM will simply terminate. Bios functions are always trapped by the 

The internal asm function SimulateInt16 is used to simulate real mode interrupts and must run in a VM86CritSec. If only an interrupt must be simulated, the monitor, which is written in asm, can call it.

A redirection map (unrelated to virtual mode extentions)

## Future Improvements

VME support could be added so that 16-bit ISRs can run directly.

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

### Memblock4k

This indicates the memory given to the program starting from zero. This variable can change. Memory is dynamically allocated and mapped whenever it is used in 4K blocks. The UMA allocation can be configured

Only the low byte is used. The high byte is for configuration. Bit 7 defines "allocate VRAM on startup". Bit 6 defines "allocate block on out-of-memory". This feature captures invalid memory access and allocates memory for a program.

### 0: Task mode

0=Exclusive
1=Concurrent

An exclusive VM halts the scheduler's normal operation and only runs one program. All interrupts

### 1: Video configuration

|F|V|

V: Use video (if 0 rest are unused)
F: Change mode and enter fullscreen

If F=0,

If a video mode is changed using INT 10H, the interrupt is trapped and the mode is forcefully switched.

### 2: BIOS, hardware, etc. directly accessible

IN and OUT instructions are emulated. BIOS calls are accessible. Video can be used, so fullscreening to DOS is possible. If bit 7 is set, exclusive tasking is implied. Note that all BIOS interrupts block the scheduler and are done with interrupts disabled. An attempt to enable or disable interrupts will simply do nothing

## VMC Examples

DOS programs are run by a system service that traps DOS API calls and redirects some of them to the kernel.

```
void VM16test()
{
	VMControl vmc;
	vmc.conf_bytes[0] = 1;
	vmc.conf_bytes[1] = ;
}
```
