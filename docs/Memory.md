# Memory Management

OS/90 supports virtual memory with paging. The MMGR can access up to 512M of physical memory (minus non-contiguities and memory holes). Programs can have access to 1G of virtual addressing space.

The memory manager is the most complicated component of OS/90.

## Detection

* INT 15H is used for memory detection.
  * AX=E801h is used to detect memory larger that 15MB.
  * AH=88h is the second function used if the previous is not implemented. This only supports up to 15MB of RAM.

* E820 is not supported as it is overkill for the target systems.

* The ISA memory hole is always assumed to be present.

## Relation with Plug and Play

The resource list allows memory ranges to be owned by a specific program. This is supposed to be used by __devices__, not software.

Information on the extended memory used by the kernel and drivers is not reported, as that is not the purpose of plug-and-play.

## Page Fault and Double Fault Handling

A page fault handler is not supposed to access memory that is not present because that would cause a double fault. The double fault handler detects if a variable indicating that a page fault was being handled is set. If it's set, the #DF handler can find out how the #DF happened because the previous context variable is never modified by exception handlers.

The double fault is unrecoverable and will lead to a panic.

## Page Frame Allocation

### Goals

The allocation strategy is very barebones but fast. It is deterministic and not slowed by fragmentation because it uses fixed-size blocks.

### Design

Fixed blocks are used. The size can be configured but is 8K by default.

### Mapping Heads

Alignment is checked for all page functions.

## Page Table Generation

A function exist for mapping addresses within a page directory. It allows mappings to cross PDE boundaries.

The page directory entry to be used in a mapping is `address >> (PAGE_SHIFT - 6)`.

## Virtual Memory

Swap files are supported. It is supposed to be present on the root directory of the boot disk and names SWAP.000. This file must be a 4K multiple in size.

The virtual memory system allows programs to use more memory space than is physically present. It also protects from OOM errors.

### Goals

Pages do not need to be constantly swapped unless the main memory is under pressure and more important things need to be prioritized. If the system swaps too much, it will thrash and slow down the system.



## Heap Management

Memory allocation functions return handles. Blocks must be frozen to get the address. Freezing maps the block to an arbitrary location in memory. GlobalAlloc blocks that are frozen map to the kernel address space or user address space depending on parameters. Allocated memory may be relocated to save allocation entries.

If GlobalAlloc returns -1, the function failed. Otherwise, it returns the address of the block.

The first argument of the FreezeBlock function is the thread ID. If it's -1, there is no process. This allows for sharing memory.

Kernel example:
```c
void AllocEG()
{
    Handle h     = GlobalAlloc(0x1000, 0);
    pbyte  block = FreezeBlock(-1, h, FRZ_R | FRZ_W);
}
```

## Virtual Memory

Previously, it was considered to determine page swapping at the process level, but this has been changed. Even high priority programs may not need certain pages to be constantly in memory.

Each program gets a page directory. The kernel/driver pages are shared with the rest of the processes. Drivers are loaded at startup so that each page directory does not need to be modified. The kernel uses its own page directory only for starting up. Multitasking will cause the kernel to share pages with the rest of the programs.

At startup, the memory manager generates the page tables for the kernel.

New page mappings can be loaded from C using inline assembly because the kernel never moves. Writing to CR3 will always refresh the TLB. If CR3 does not need to be changed, it is not written.

# Kernel API

int GlobalMap(dword pid, pvoid proc_page, pvoid to, Page c, sdword access)

Definitions:
PG_AVAIL, PG_W, PG_R, MAP_FAILED, MAP_SUCCESS

This functon modifies the page tables of a process to map somewhere else. Addresses must be page aligned. GlobalMap is useful for mapping framebuffers for DOS applications. It can be used for 32-bit processes to facillitate data sharing.

## Programming Considerations
(Updated 2022-08-05)

Programs should avoid allocating and freeing memory and rely on the .bss section more. bss is zeroes only if the kernel needs it. Allocations can be called, but they should be relatively large and page-multiples in size.

Software libraries can help with managing .bss heaps if that is necessary.

On i386 computers, the lack of INVLPG will make frequent memory freezes slow the program slightly because the TLB has to be flushed repeadedly every time a page table is modified. Software for this class of hardware should avoid freezing often.
