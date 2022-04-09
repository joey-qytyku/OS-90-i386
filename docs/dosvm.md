# DOS Virtual Machines

DOS virtual machines can be created by the operating system and other supervisory components to emulate real mode programs with different options. Direct hardware,  memory, and BIOS access can be configured by the requesting module. Tasking can also be configured to be exclusive or concurrent and video memory can be directly accessed or mapped to a virtual framebuffer. Only standard VGA supports this feature.

# Abstract

DOS virtual machines are processes that run in VM86 mode. They do not have access to kernel API functions and are isolated from the system.

# DOS emulation

The DOS kernel is not emulated, but DOS functions are redirected to the kernel in the physical conventional memory through the real mode IVT, which is mapped into the virtual VM86 address space. All configurations of a virtual machine can access the DOS system.

# API

CreateVM16(struct VMControl vmctl)

## VM Control block

```c++
struct VMControl
{
	byte	conf_bytes[16];
	byte	memblock4k;
};
```

conf[0] is defined as

### 0: Task mode

0=Exclusive
1=Concurrent

An exclusive VM halts the schedulers normal operation and only runs one program.

### 1: Use video

0=No video

### 2: Video mode set

0=Use virtual framebuffer
1=Change video mode

### 3: BIOS, hardware, etc. directly accessible

IN and OUT instructions are emulated. BIOS calls are accessible. Exclusive tasking is implied. Video can be used, so fullscreening to DOS is possible.
