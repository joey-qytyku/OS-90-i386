# Memory Management

OS/90 supports virtual memory with paging. The MMGR can access 1GB of physical memory. Programs have access to  1G of virtual addressing space.

## Page Frame Allocation

A buddy-like algorithm is used to implement page frame allocation. A few entries are in the kernel and used to bootstrap everything else.

## Virtual Memory

Virtual memor is handled at the process level. The kernel determines swapability.

# Kernel API

GlobalMap(dword pid, void *proc_page, void *to, page c, int access)

PG_AVAIL
PG_W
PG_R

