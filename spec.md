# Design

The OS does not use any special abstractions. It is a DOS-inspired OS. The VFS is letter-based. File redirection and standard streams are supported.

# Bootloader

The OS boots using MSDOS. The OS is contained in a directory which is defined in CONIG2.SYS. This directory can be on the DOS partition or it can be external (latter is recommended). The kernel defines the boot partiton as drive A. Other drives go after that. The bootloader does not read the configuration file

The program is an executable which may be a COM file or EXE. It should be run using the command name LOADER32 from AUTOEXEC.BAT This program will gather informtion, enable the A20 gate, switch to unreal mode, and read the kernel above 1M. PnP can be configured later. The kernel name is KERNEL.BIN and is a flat binary.

Once the kernel is loaded into memory, the loader jumps to 1M while in real mode and trashes the segment used by unreal mode. The kernel must use its own GDT and IDT. Interrupts are disabled upon entry. ES:BX point to the information structure, which will remain resident. The kernel re-enable interrupts while it is in real mode and should do this only in protected mode. The kernel startup file is no larger that 2K.

To facillitate SMP support through APIC, a region of memory is allocated 4096 bytes long (256 paragraphs) for trampoline code. It does not need to be used for this purpose but is required for booting.

## Information structure

The following is the definition of the bootloader information. It is byte aligned.

```
bootinfo STRUCT
	bootdsk BYTE	?
	memconv WORD	?
	memextn WORD	? ; Number of pages
	apmpres BYTE	?
	cs32	WORD	? ; Real mode segment of APM
	ip32	DWORD	? ; Offset to the APM interface
	apmds	WORD	? ; Data segment base, 16/32
	cslen   DWORD	? ; Length16:Length32
	dslen   WORD	?
	free4k  WORD	? ; Segment of the allocated data
bootinfo ENDS
```

# File streams

File redirection is used for the multitasking window environment to allow for virtual consoles.

# Interrupts and exceptions

The stack is always 4-byte aligned. Upon entry, an interrupt handler will fetch the address of the trap frame. The SS and ESP fields are only valid if a the last context was a user program. A variable keeps track if the current mode: kernel, interrupt, and user. Interrupts are special contexts, but are ring 0 and are not isolated from kernel.

Exceptions can sometimes return an error code.

# Multitasking

Co-operative multitasking is used by the OS. This means that only one program can run at a time, but more than once can be loaded in memory. A program must voluntarily yield to its forker, or it can terminate. A single task running does not block everything.

If a process relinquishes, it will not terminate, but it stays resident until another process runs it. That process becomes its parent. Processes cannot be orphaned and terminate if another program dies. Programs can respond to external events.

Operations:
RELQs- Switch to the caller
ENTR - Switch to an active process
FORK - Start executing a new program

The process control blocks contain the registers, which are saved when yielding the program and restored for entering. Each PCB is backlinked to the caller program.

```c++
typedef struct task {
	long eax, ebx, ecx, edx, esi, edi, ebp, esp;
	char fpenv[103];
	char active;
	long ret_pid;
};
```

# System Calls

int 3Ch is used to access the API. EDX is the function code and is truncated to an 8-bit index. ESI and EDI are the primary argument registers. This is not the case for all system calls.

System calls finish in their entirety and cannot be pre-empted.
