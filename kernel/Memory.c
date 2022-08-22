/*
     This file is part of OS/90.

    OS/90 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

    OS/90 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with OS/90. If not, see <https://www.gnu.org/licenses/>. 
*/

#include <DriverLib/Drivers.h>
#include <Type.h>
#include <Linker.h>
#include <Memory.h>

#define CHECK_ALIGN(address, if_unaligned)\
if ((dword)address & 0xFFF != 0)\
    {goto if_unaligned}      /* No misleading IF warning */

// The ISA memory hole is a 1M space that is sometimes
// reserved for some ISA cards. It is at F00000-FFFFFF
// PCs that do not need it may have it anyway or
// an option to enable. The target
// platform (PCs from 1990-1999) certainly have an ISA
// memory hole, so it is assumed to be present.

static DWORD memory_after_15M; // Extended memory after the hole
static DWORD memory_after_1M;  // Extended memory between hole and 1M

//  CONV  RES   EXT  ISA  EXT
// [640K][384K][15M][1M][xM...]

static dword AddrAlign(PVOID addr, DWORD bound)
{
    return ((dword)addr + bound - 1) & ~(bound-1);
}

// Allocate nodes from a node array to the new head being initialized.
//
