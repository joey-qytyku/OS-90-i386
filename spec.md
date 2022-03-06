# Design

The OS does not use any special abstractions. It is a DOS-inspired OS. The VFS is letter-based. File redirection and standard streams are supported.

# Boot

The kernel is a file that will likely be >64K. The loader should run in the HMA using loadfix. LOADER.COM will open the KERNEL.BIN file and write every 4K page in it to the conventional memory at 0x1000.

# File streams

File redirection is used for the multitasking window environment to allow for virtual consoles.

# Interrupts and exceptions

The stack is aligned to eight upon entry. An interrupt handler will fetch the address of the trap frame. The SS and ESP fields are only valid if a the last context was a user program. A variable keeps track if the current mode: kernel, interrupt, and user. Interrupts are special contexts, but are ring 0 and are not isolated from kernel.

Exceptions can sometimes return an error code.

# Memory Model

The kernel is loaded at 1M. MS-DOS is used to boot the operating system but there are no dependencies with it. MS-DOS should not be loaded high or it may be overwritten. DOS is "clobbered" by the procedure and the OS cannot exit.



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
