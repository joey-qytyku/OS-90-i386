/*
     This file is part of OS/90.

    OS/90 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

    OS/90 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with OS/90. If not, see <ttps://www.gnu.org/licenses/>.
*/

#ifndef V86_H
#define V86_H

#include <Platform/IA32.h>
#include <Type.h>


#define CAPT_HND   0 /* Handled captured trap*/
#define CAPT_NOHND 1 /* Did not handle */
#define CAPT_NUKE  2 /* Or'ed with other option */

extern VOID ScOnErrorDetatchLinks(VOID)
extern VOID ScVirtual86_Int(PTRAP_FRAME, BYTE)

extern void ScMonitorV86(PTRAP_FRAME);
extern void ScEnterV86(PTRAP_FRAME);
extern void ScShootdownV86(VOID);     // Defined in vm86.asm

typedef struct self_V86_Chain_Struct
{
    self_V86_Chain_Struct handler;  // Set the handler
    PVOID next;     // Initialize to zero
}V86_Chain_Struct,*PV86_CHAIN_LINK;

#endif /* V86_H */
