#include <Farcall.h>
#define MAX_SUPPORTED_FAR_CALLS 8

WORD hnd_inx;
DOS_FCALL_HANDLER fc_handlers[MAX_SUPPORTED_FAR_CALLS];
V86_CHAIN_LINK    v86_report_links[MAX_SUPPORTED_FAR_CALLS];

// This function is called by the page fault handler when the proper
// conditions are met. Those conditions are not the concern of this function.
//
VOID HandleFcallAfterPF(WORD offset_from_bios_rom, PTRAP_FRAME tf)
{
}

//
// Returns the offset from the BIOS rom space or -1 on error
//
// report_handler is a v86 chain link that implements the DOS calls
// that report the address of the far call interface to the program.
//
// Returns 32-bit seg:off pair relative to BIOS F000:0000
//
DWORD ScInsertFarCallHandler(
                            VINT              vector,
                            V86_HANDLER       report_handler,
                            DOS_FCALL_HANDLER fcall_handler
){
    if (hnd_inx+1 == MAX_SUPPORTED_FAR_CALLS)
        return -1;

    ScHookDosTrap(vector, &v86_report_links[hnd_inx], report_handler);

    fc_handlers[hnd_inx] = fcall_handler;
    hnd_inx++;

    return hnd_inx;
}
