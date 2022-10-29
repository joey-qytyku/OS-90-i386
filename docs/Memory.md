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

A two-layer linked list is used. Heads are pointers to the allocation chain.

The kernel resizes a single chain for the purpose of bootstrapping head and chain allocation.

Bootstrap Memory MMS : Post-bootstrap MMS

Memory Managment Structures : Usable memory

Because the kernel has to allocate memory to allocate memory, MMS causes some internal fragmentation and waste, but MMS are very small structures.

Tails are linked lists. Because fixed blocks are used, they simply need to indicate that they exist and the next chain. Tails are always stored at DWORD aligned addresses relative to a page aligned address and are tagged pointers.

The fist two bits:
00=Not present
01=Present
10=End of sequence, not present
11=End of sequence, present

## Example

Allocating 64 KiB with 16K blocks would require five DWORDS. Worst case for fragmentation is that another 16K have to be allocated for the new DWORDS, leading to 80 KiB being allocated.

As explained above, the memory manager is not deterministic in fragmentation and can change from being efficient to wasteful at arbitrary intervals.

The larger the block size, the higher the internal fragmentation, and less frequent the waste intervals for the head and chain space.

Speed is more deterministic and higher because of fixed blocks.

## Programming Advice

The memory manager should normally be transparent to userspace programmers. For kernel mode software, memory can only be controlled using direct memory calls. The block size can by dynamically recieved by drivers.

### Functions Supported

Allocating

Deleting
Resizing (+/-)

## Virtual Memory

Swap files are supported. It is supposed to be present on the root directory of the boot disk and names SWAP.000. This file must be a 4K multiple in size.

The virtual memory system allows programs to use more memory space than is physically present. It also protects from OOM errors.

### Goals

Pages do not need to be constantly swapped unless the main memory is under pressure and more important things need to be prioritized. If the system swaps too much, it will thrash and slow down the system.

## Address Spaces

The memory manager will keep track of which parts of physical memory are in use and by what. It will also be able to map allocation chains to arbitrary locations. The issue now is allocating address spaces for kernel software so that a simple malloc-like call would be possible.

Userspace processes cannot invoke memory allocation and should "know" exactly how much they need or extend their total memory. The .bss section should be used for a static "heap".

 Memory sharing is possible, however. In theory, one program can be a memory broker for other processes (IDK about this). When a process is loaded into memory, it is mapped flat into the virtual address space starting at 1M (anything lower makes the executable invalid).

The kernel needs the ability to map memory to arbitrary locations.

## Virtual Memory

Virtual memory is implemented in the kernel.

Blocks can be

# Kernel API

int GlobalMap(dword pid, pvoid proc_page, pvoid to, Page c, sdword access)

Definitions:
PG_AVAIL, PG_W, PG_R, MAP_FAILED, MAP_SUCCESS

This functon modifies the page tables of a process to map somewhere else. Addresses must be page aligned. GlobalMap is useful for mapping framebuffers for DOS applications. It can be used for 32-bit processes to facillitate data sharing.

## Programming Considerations
(Updated 2022-08-05)

Programs should avoid allocating and freeing memory and rely on the .bss section more. Allocations can be called, but they should be relatively large and page-multiples in size.

Software libraries can help with managing .bss heaps if that is necessary.

On i386 computers, the lack of INVLPG will make frequent memory freezes slow the program slightly because the TLB has to be flushed repeadedly every time a page table is modified. Software for this class of hardware should avoid freezing often.
