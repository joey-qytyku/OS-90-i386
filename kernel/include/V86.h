#ifndef V86_H
#define V86_H

#include <Platform/IA32.h>
#include <Type.h>

extern void ScMonitorV86(PTRAP_FRAME);
extern void ScEnterV86(PTRAP_FRAME);
extern void ScShootdownV86(VOID);     // Defined in vm86.asm

#define CAPT_HND   0 /* Handled captured trap*/
#define CAPT_NOHND 1 /* Did not handle */
#define CAPT_NUKE  2 /* Or'ed with other option */

typedef struct $Self
{
    $Self   handler;  // Set the handler
    PVOID   next;     // Initialize to zero
}V86_Chain_Struct,*PV86_CHAIN_LINK;

#endif /* V86_H */
