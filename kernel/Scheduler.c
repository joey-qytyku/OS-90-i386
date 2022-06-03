#if 0

Everything process and control flow related
* Scheduler
* V86
* Interrupt handling

#endif

#include <Scheduler.h>
#include <Type.h>
#include <IA32.h>

byte vm86_caused_gpf=0, emulate_svi=0;
dword current_proc=0;

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


// Technically not a far pointer, but reminds me of DOS
inline void *MK_FP(word seg, word off)
{
    return (void*)((seg<<4) + off);
}

void bPeek86(word seg, word off) {return *(byte*)Segment(seg,off);}
void wPoke86(word seg, word off) {return *(byte*)Segment(seg,off);}

void IntDispatch()
{}

void MonitorVM86()
{
}

/* Scheduler code */

void InitScheduler(void)
{
    C_memset(&main_tss.iopb_deny_all, '\xFF', 8192);
}

