# Memory Management

OS/90 supports virtual memory with paging. The MMGR can access up to 512M of physical memory (minus non-contiguities and memory holes). Programs can have access to 1G of virtual addressing space.

The linked list memory management structures are used to determine which page frames are in use and free. These structures can be converted to page directories and tables with page table compositing routines.

# Detection

* INT 15H is used for memory detection.
  * AX=E801h is used to detect memory larger that 15MB.
  * AH=88h is the second function used if the previous is not implemented. This only supports up to 15MB of RAM.

* ACPI E820 is not supported as it is overkill for the target systems.
* The ISA memory hole is always assumed to be present.

# Relation with Plug and Play

The resource list allows memory ranges to be owned by a specific program. This is supposed to be used by __devices__, not software.

Information on the extended memory used by the kernel and drivers is not reported, as that is not the purpose of plug-and-play.

# Page Fault and Double Fault Handling

A page fault handler is not supposed to access memory that is not present because that would cause a double fault. The double fault handler detects if a variable indicating that a page fault was being handled is set. If it's set, the #DF handler can find out how the #DF happened because the previous context variable is never modified by exception handlers.

The double fault is unrecoverable and will lead to a panic.

# Page Frame Allocation

It is important to emphasize that this is a very low-level interface exposed only to kernel code. This is not a simple malloc/free. Which page frame allocation does is allow software to determine which pages are free for use and allow them to be mapped to virtual addresses.

Here, I will explain different possibilities and what I think of them.

## Possible Solution: Keep track of all blocks with structures

The advantage is that this is really easy. Allocating blocks is literally just a simple for-loop. External fragmentation is non-existent.

Suppose a computer has 8 MiB of memory. 640 KiB is reserved for DOS, and 384 KiB is used by the kernel, and the HMA is ignored. There are 7 MiB ready for use. If blocks of 16 KiB are used, we would need 448 block entries.

These block entries would have to store a reference to an object that represents an allocation, maybe a handle. Suppose this is 32-bit.

The disadvantage is that a fixed percentage of memory is constantly being utilized, regardless of how much total memory is available and current use. If a PC has 64 MB of RAM, 4032 entries would be needed.

### Algorithms

To allocate a new chain of blocks, a simple for loop iterates though the block array and searches for free ones. When a free node is detected, a pointer to an allocation root structure can be inserted.

### Mapping Memory

When blocks are allocated, they must be mapped to virtual memory to be used, as chains are not always contiguous.

Memory is mapped by the handle to an arbitrary virtual address. On the last block, not all the pages need to be mapped.

### Page tables and directories

There is only one page directory. One directory can contain up to page table 1024 references, and each table can contain 1024 page references. A single page table can map 4M of memory, so 17 pages, or 68 KiB, is enough for 64 MB.

Each process has its own page directory. The page tables for the kernel and drivers is static because driver unloading is currently not suppoted.

The paging structures should not need to be mapped to virtual address space. They can be accessed directly because they fit in a page frame.

## Programming Advice

## Virtual Memory

Swap files are supported. It is supposed to be present on the root directory of the boot disk and named SWAP.000. This file must be a 4K multiple in size.

The virtual memory system allows programs to use more memory space than is physically present. It also protects from OOM errors.

### Goals

Pages do not need to be constantly swapped unless the main memory is under pressure and more important things need to be prioritized. If the system swaps too much, it will thrash and slow down the system.

## Address Spaces

The memory manager will keep track of which parts of physical memory are in use and by what. It will also be able to map allocation chains to arbitrary locations. The issue now is allocating address spaces for kernel software so that a simple malloc-like call would be possible.

Memory sharing is possible, however. In theory, one program can be a memory broker for other processes (IDK about this). When a process is loaded into memory, it is mapped flat into the virtual address space starting at 1M+64K (anything lower makes the executable invalid).

## Relation with Threads

Each process gets a page directory.

Each thread gets an allocation head. If the process needs more memory, it can resize the head to get it.

## Virtual Memory

Virtual memory is implemented in the kernel.

# Memory Allocation

Malloc and free are the convenient functions for allocating memory in C. The key point of the design is that malloc returns a pointer and free takes a pointer. All modern operating systems support a similar model of memory allocation.

Another way to do this is to use integer handles to refer to memory blocks. Handles are better for avoiding memory fragmentation as blocks can be re-shuffled by the kernel to make space and reduce fragmentation. This is also much more convenient for DPMI support, as the spec uses handles.

In OS/90, memory allocation uses resizable heaps which every program gets. This allows defragmentation to happen at a per-program basis. The userspace libraries will allow for heap creation. Using DPMI will allocate memory only to the default heap. Handles are garaunteed to be interchangable between the two.

The kernel must allocate memory in page blocks.

## Kernel and User

The kernel has to implement memory allocation using handles in order to support DPMI and XMS. The userspace does not implement it. All kernels need a way to allocate memory in the same way that the userspace can.

Heaps require specifying a virtual address range where blocks will be mapped. The kernel uses an address above C0000000. All drivers should use this heap.

## Block Structure

Blocks have headers with the following information:

```c
typedef struct _MEMBLOCK_CTL_HEADER {
  DWORD size_bytes;
  DWORD flags;
  DWORD handle;
  PVOID cache_addr;
  _MEMBLOCK_CTL_HEADER next_header
}MEMBLOCK_CTL_HEADER;
```

If bit zero of flags is set, it indicates that the block is page-discontiguous. Such blocks cross page boundaries.

If bit 1 of the flags is set, the block is frozen 

flags:2 = 1 indicates that the cached address it still valid. The cached address is returned by the freeze function.

The memory area starts immediately after the header.

## Block Reshuffling

Blocks are sometimes reshuffled to make more space. To determine an optimal way to do this algorithm:

Each symbol represents a different memory block. Spaces are page boundaries.

##|@@|!!|!!|!!

I delete @@.

##|  |!!|!!|!!

I then allocate @@@@. I can do something here.

@@|@@|!!|!!|!!|##|

This is the desired output. The concept is that small blocks of memory can be relocated to reduce fragmentation. Note that I can only reduce it. There is no perfect method.

Defragmentation should look to the smaller blocks (<4K) and attempt to nearly fill entire pages with them. Alignment is never garaunteed, but will probably be 32-bit for better performance.

Defragmenting is a slow process that requires copying memory, so it should happen rarely. It can happen with fixed intervals or a more complicated algorithm can be implemented.

For example, if there is a large deviation between allocation sizes, a defrag is more likely. 

# Dynamic structures

## Vector
