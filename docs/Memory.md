# Memory Management

OS/90 supports virtual memory with paging. The MMGR can access up to 1GB of physical memory (minus non-contiguities). Programs can have access to 1G of virtual addressing space.

## Page Frame Allocation

The start of the available memory is found by looking above the kernel BSS section. Linked lists are used to keep track of page allocations. A head points to the start of a linked list.

A list element represents a certain number of pages at a relative location (to the previous element). Calculating physical addresses is expensive. For this reason, a small software cache holds the recently calculated effective addresses matched with their heads (?). It can be configured to be larger, but increasing the size requires the kernel to loop through more elements.

Allocation tails can be mapped to page-aligned addresses in the memory for both user processes and the kernel. These addresses can be arbitrary or fixed. These functions are not exposed to drivers

MapTail()

Alignment is checked for all page functions.

## Page Table Generation

A function exist for mapping addresses within a page directory. It allows mappings to cross PDE boundaries.

The page directory entry to be used in a mapping is `address >> (PAGE_SHIFT - 6)`.

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

New page mappings can be loaded from C using inline assembly because the kernel never moves. Writing to CR3 will always refresh the TLB. iF CR3 does not need to be changed, it is not written.

# Kernel API

int GlobalMap(dword pid, pvoid proc_page, pvoid to, Page c, sdword access)

Definitions:
PG_AVAIL, PG_W, PG_R, MAP_FAILED, MAP_SUCCESS

This functon modifies the page tables of a process to map somewhere else. Addresses must be page aligned. GlobalMap is useful for mapping framebuffers for DOS applications. It can be used for 32-bit processes to facillitate data sharing.

