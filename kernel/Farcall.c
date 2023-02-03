#include <>


//
// Far call handler does not need to return anything. The kernel
// knows exactly which one to call by index
//
typedef VOID (*DOS_FCALL_HANDLER) (PTRAP_FRAME*);

//
// To make the interface simpler, the V86 chain link struct
// is embedded here. This structure is part of a linked list.
// The difference is that it is indexed by number rather than probing
// functions iteratively as with V86.
//

typedef struct {
  V86_CHAIN_LINK        v86_cl;
  DOS_FAR_CALL_HANDLER  handler;
  WORD                  rom_space_offset;
}FARCALL_CONF;

static WORD current_farcall = 0;
const PVOID bios_rom_space = 0xF0000;


//
// This function is called by the page fault handler when the proper
// conditions are met.
//
HandleFcallAfterPF(WORD eip)
{
  // EIP represent
}


//
// Brief: Far calls are intercepted through the page fault handler.
// Regular DOS programs may access XMS and other far call API's. The
// first step is to capture the multiplex interrupt
//
ScCreateDosFarCallHandler(
                          FARCALL_CONF  fcall_cfg,
                          STATUS (*report_handler)(),
                          DOS_FCALL_HND fcall_handler;
){
}
