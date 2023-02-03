#include <>

typedef DOS_FAR_CALL_HANDLER;

//
// To make the interface simpler, the V86 chain link is embedded
//

typedef struct {
  V86_CHAIN_LINK        v86_cl;
  DOS_FAR_CALL_HANDLER  handler;
  WORD                  rom_space_offset;
}FARCALL_CONF;

static WORD current_farcall = 0;
const PVOID bios_rom_space = 0xF0000;

HandleFcallAfterPF(WORD eip)
{
}


//
// Brief: Far calls are intercepted through the page fault handler.
// Regular DOS programs may access XMS and other far call API's. The
// first step is to capture the multiplex interrupt
//
// address_report_vint_link: A V86 chain link that reports
// fcall_conf_struct: A structure to hold the handler address
//
ScCreateDosFarCallHandler(
                          )
{
}
