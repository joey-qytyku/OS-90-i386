# XMS Emulation

XMS has two features that make accessing it through protected mode annoying, memory parameters and far calls. It must be implemented in the kernel because of this.

XMS can be called before OS/90 is started. Blocks already allocated are left alone when allocating memory for OS/90. DOS programs running in as protected mode tasks access a controlled far call interface. The only way for XMS to be implemented in 32-bit mode is to check for a page fault when accessing invalid memory. Then the far call address is detected by the #PF handler and dispatched to a far call chain, similar to the V86 INT chain.

# Specifics

Implementation of the XMS API is integrated with the memory manager. XMS assigns handles to allocated blocks.

Making an XMS call from 32-bit mode to DOS is a terrible idea and should never happen.

## Method of Hooking

V86 hooking is used to return proper results when checking for XMS with INT 2Fh. The far call address returned must be obtained from the kernel. The address is at an unmapped section of the ROM space. The offset from the start of the ROM space indicates the chain link index for the page fault handler.

Remember that the BIOS ROM is never mapped to a V86 process. Such processes have completely isolated address spaces and TLB refreshing is required.

The far call chain provides one handler for every possible far cal address. A kernel routine increments the offset every time a new far call is added.

# XMS as an OS Memory Manager

Windows 3.1 on 286 used HIMEM.SYS to allocate memory during protected mode. This required sending a reset signal and using a BIOS re-enter address feature. This is possible with V86 and I would have done it this way too if it was possible. For page-based virtual memory, XMS is completely impossible to use. It does not allow for aligned and fragmented allocation.
