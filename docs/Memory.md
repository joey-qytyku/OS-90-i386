# Memory Management

OS/90 supports virtual memory with paging. The MMGR can access up to 1GB of physical memory (minus non-contiguities). Programs can have access to 1G of virtual addressing space.

## Page Frame Allocation

The start of the available memory is found by looking above the kernel BSS section. Linked lists are used to keep track of page allocations. A head points to the start of a linked list.

A list element represents a certain number of pages at a relative location (to the previous element). Calculating physical addresses is expensive. For this reason, a small software cache holds the recently calculated effective addresses matched with their heads (?). It can be configured to be larger, but increasing the size requires the kernel to loop through more elements.

Allocation tails can be mapped to page-aligned addresses in the memory for both user processes and the kernel. These addresses can be arbitrary or fixed. These functions are not exposed to drivers

MapTail()

## Heap Management

Memory allocation functions return handles. Blocks must be frozen to get the address. Freezing maps the block to an arbitrary location in memory. GlobalAlloc blocks that are frozen map to the kernel address space or user address space depending on parameters.

Kernel example:
```c
void AllocEG()
{
    Handle h     = GlobalAlloc(0x1000, 0);
    pbyte  block = FreezeBlock(h, FRZ_R | FRZ_W);
}
```

## Virtual Memory

Virtual memories for 32-bit processes are handled at the process level. The kernel determines swapability and priority by the frequency of IO requests.

Processes can maximize performance by doing as much work as possible with files, because higher IO load will also give more CPU time and reduce page swapping. The system can also be hacked with nonsense FS operations.

# Kernel API

int GlobalMap(dword pid, void *proc_page, void *to, page c, int access)

Definitions:
PG_AVAIL, PG_W, PG_R, MAP_FAILED, MAP_SUCCESS

This functon modifies the page tables of a process to map somewhere else. Addresses must be page aligned. GlobalMap is useful for mapping framebuffers for DOS applications. It can be used for 32-bit processes to facillitate data sharing.

