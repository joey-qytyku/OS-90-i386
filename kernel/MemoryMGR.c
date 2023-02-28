#include <Memory.h>

DWORD MmAllocateBlock(HANDLE heap_hnd, DWORD bytes);

VOID MmDeleteBlock()

PVOID MmFreezeMem(HANDLE heap_hnd, HANDLE block_hnd);
VOID MmReleaseMem(HANDLE heap_hnd, HANDLE block_hnd)

HANDLE MmCreateHeap(PVOID virtual_addr);
