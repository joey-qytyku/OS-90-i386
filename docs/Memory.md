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

The memory manager should normally be transparent to userspace programmers. For kernel mode software, memory can only be controlled using direct memory calls. The block size can by dynamically recieved by drivers.

### Functions Supported

* Allocating
* Getting the address
* Deleting
* Resizing (+/-)

## Virtual Memory

Swap files are supported. It is supposed to be present on the root directory of the boot disk and named SWAP.000. This file must be a 4K multiple in size.

The virtual memory system allows programs to use more memory space than is physically present. It also protects from OOM errors.

### Goals

Pages do not need to be constantly swapped unless the main memory is under pressure and more important things need to be prioritized. If the system swaps too much, it will thrash and slow down the system.

## Address Spaces

The memory manager will keep track of which parts of physical memory are in use and by what. It will also be able to map allocation chains to arbitrary locations. The issue now is allocating address spaces for kernel software so that a simple malloc-like call would be possible.

Memory sharing is possible, however. In theory, one program can be a memory broker for other processes (IDK about this). When a process is loaded into memory, it is mapped flat into the virtual address space starting at 1M (anything lower makes the executable invalid).

## Relation with Threads

Each process gets a page directory.

Each thread gets an allocation head. If the process needs more memory, it can resize the head to get it.

## Virtual Memory

Virtual memory is implemented in the kernel.

# Proper Allocation

Malloc and free are the convenient functions for allocating memory in C. The key point of the design is that malloc returns a pointer and free takes a pointer. All modern operating systems support a similar model of memory allocation.

Another way to do this is to use integer handles to refer to memory blocks. Using pointers is easier for programming as it does not require memorizing the handle from allocation and the address from the freeze function.

## Implementation

Memory allocation must be implemented separately for userspace and the kernel.

# Dynamic structures

## Vector

## Programming Considerations
(Updated 2022-08-05)

Programs should avoid allocating and freeing memory and rely on the .bss section more. Allocations can be called, but they should be relatively large and page-multiples in size.

Software libraries can help with managing .bss heaps if that is necessary.

On i386 computers, the lack of INVLPG will make frequent memory freezes slow the program slightly because the TLB has to be flushed repeadedly every time a page table is modified. Software for this class of hardware should avoid freezing often.
