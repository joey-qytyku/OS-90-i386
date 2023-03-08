#include <Farcall.h>
#define MAX_SUPPORTED_FAR_CALLS 16


WORD hnd_inx = 0;
DOS_FCALL_HANDLER fc_handlers[MAX_SUPPORTED_FAR_CALLS];
V86_CHAIN_LINK    v86_report_links[MAX_SUPPORTED_FAR_CALLS];

//
// This function is called by the page fault handler when the proper
// conditions are met. Those conditions are not the concern of this function.
//
VOID HandleFcallAfterPF(WORD offset_from_bios_rom, PTRAP_FRAME tf)
{
}

DWORD ScInsertFarCallHandler(DOS_FCALL_HANDLER fcall_handler)
{
    if (hnd_inx+1 == MAX_SUPPORTED_FAR_CALLS)
        return -1;

    fc_handlers[hnd_inx] = fcall_handler;
    hnd_inx++;

    return 0xF000*16 + hnd_inx;
}
