# Memory Management

OS/90 supports virtual memory with paging. The MMGR can access 1GB of physical memory. Programs have access to  1G of virtual addressing space.

## Page Frame Allocation

A buddy-like algorithm is used to implement page frame allocation. A few entries are in the kernel and those are used to bootstrap.

## Virtual Memory

Virtual memories for 32-bit processes are handled at the process level. The kernel determines swapability and priority by the frequency of IO requests.

Processes can maximize performance by doing as much work as possible with files, because higher IO load will also give more CPU time and reduce page swapping. The system can also be hacked with nonsense FS operations.

# Kernel API

int GlobalMap(dword pid, void *proc_page, void *to, page c, int access)

Definitions:
PG_AVAIL, PG_W, PG_R, MAP_FAILED, MAP_SUCCESS

This functon modifies the page tables of a process to map somewhere else. Addresses must be page aligned. GlobalMap is useful for mapping framebuffers for DOS applications. It can be used for 32-bit processes to facillitate data sharing.

