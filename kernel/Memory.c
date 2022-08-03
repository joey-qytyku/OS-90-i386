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

#define NUM_TAILS 1024

typedef struct SelfAllocNode {
    Page size;
    struct SelfAllocNode *frontln, *backln;
}AllocNode,*PAllocNode;

typedef struct SelfHead {
    bool user;
    PAllocNode head;
}Head,*PHead;

typedef struct {
    pvoid addr;
}Map;

static dword AddrAlign(pvoid addr, dword bound)
{
    return ((dword)addr + bound - 1) & ~(bound-1);
}

__DRVFUNC Handle MeGlobalMap(pvoid start, pvoid to, dword size)
{
}
__DRVFUNC void MeGlobalDelMap(Handle m);

/* High level function for heap management w/buckets */

__DRVFUNC Handle MeGlobalAlloc(dword size, bool user);
__DRVFUNC void   MeFreezeBlock(Handle h, dword flags);

// What about mapping physical memory to kernel space?
// to an arbitrary location?

void PageFault()
{}

void InitMem(dword mem1k_after_1m,
dword mem_after_memhole)
{
}
