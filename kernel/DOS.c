#include <Platform/IA32.h>
#include <Scheduler.h>
#include <Farcall.h>
#include <Debug.h>
#include <Type.h>
#include <V86.h>

//
// DOS server and DPMI implementation
//

static V86_CHAIN_LINK lnk = { 0 };
static DWORD dpmi_entry_farcall;


static STATUS DpmiRealModeApiHandler(PTRAP_FRAME tf)
{
    if ((WORD)tf->regs.eax != 0x1687)
        return CAPT_NOHND;
}

//
// When a program attempts to switch to protected mode, this function will run
// The program will run where it started executing, but instead it will be in
// 32-bit mode. The GDT ring-3 code segment will lets us do this.
//
// We will have to mangle the process context to make this work
//
static VOID DpmiProcessEnterProtectedMode(VOID)
{
    // Get the process control block of the current process
}

VOID KeInit_DOS_Server()
{
    // Hook INT 2Fh
    ScHookDosTrap(0x2F, &lnk, DpmiRealModeApiHandler);
    ScInsertFarCallHandler(DpmiHandleEntryFarcall);
}
