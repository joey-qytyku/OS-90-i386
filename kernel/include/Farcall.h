#ifndef FARCALL_H
#define FARCALL_H

//
// To make the interface simpler, the V86 chain link struct
// is embedded here. This structure is part of a linked list.
// The difference is that it is indexed by number rather than probing
// functions iteratively as with V86.
//

#include <V86.h>
#include <Type.h>

//
// Far call handler does not need to return anything. The kernel
// knows exactly which one to call by index
//
typedef VOID (*DOS_FCALL_HANDLER) (PTRAP_FRAME);

#endif
