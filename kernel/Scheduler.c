#include <Scheduler.h>
#include <Type.h>

byte vm86_caused_gpf=0, emulate_svi = 0;
dword current_proc=0;

// After a context switch, this becomes significant
Mode last_mode = KERNEL;

// used by vm86.asm, automatically cleared
// by the gpf handler when it is set by VM86.asm

void HandleGPF(dword error_code)
{
    /*
     * The error code is always zero if it is not
     * segment related. There are no zero selectors
    */

   // Is it from the kernel?
   // If so PANIC

    // Is the GPF from VM86?

    // If not, terminate the current task
}


inline void
*MK_FP(word seg, word off)
{
    return (void*)((seg<<4) + off);
}

void bPeek86(word seg, word off) {return *(byte*)Segment(seg,off);};
void wPoke86(word seg, word off) {return *(byte*)Segment(seg,off);};

#if 0

16-bit hard disk IO should work with virtual memory
but floppy disk IO probably will not because ISA DMA
requires physical addresses.

Should I use the HMA for this purpose? I can then copy
the data where it is needed

#endif

void MonitorVM86()
{
}
