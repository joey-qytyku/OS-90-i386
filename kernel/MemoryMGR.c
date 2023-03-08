#include <V86.h>
#include <Memory.h>

DWORD MmAllocateBlock(HANDLE heap_hnd, DWORD bytes);

VOID MmDeleteBlock();

PVOID MmFreezeMem(HANDLE heap_hnd, HANDLE block_hnd);
VOID MmReleaseMem(HANDLE heap_hnd, HANDLE block_hnd);

HANDLE MmCreateHeap(PVOID virtual_addr);

//
// Note: NOT THREAD SAFE. Do not execute in an ISR.
// If fails, segment returned is FFFF, or -1
//
WORD MmAllocateDosMemory(WORD paragraphs)
{
    TRAP_FRAME tf;

    tf.regs.eax = 0x4800;
    tf.regs.ebx = paragraphs;

    // INIT TRAP FRAME!

    ScVirtual86_Int(&tf, 0x21);

    if (tf.eflags & 1)
    {
        // If the DOS call fails, it returns the maximum block size
        // with available memory. This function assumes the caller wants that
        // exact amount of memory
        return 0xFFFF;  // Error
    }
    return tf.regs.eax;
}

VOID MmFreeDosMemory(WORD segment)
{
    TRAP_FRAME tf;

    ScVirtual86_Int(&tf, 0x21);
}
