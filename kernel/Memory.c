/*+===================================================================

Copyright (c) Joey Qytyku 2022

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Memory manager. Implements low-level page frame allocation
and high level heap management for kernel and user

===================================================================+*/

#include <Type.h>
#include <Linker.h>
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
    pvoid    addr;
}Map;

static dword AddrAlign(pvoid addr, dword bound)
{
    return ((dword)addr + bound - 1) & ~(bound-1);
}

__DRVFUNC Handle GlobalMap(pvoid start, pvoid to, dword size)
{
}
__DRVFUNC void   GlobalDelMap(Handle m);

/* High level function for heap management w/buckets */

__DRVFUNC Handle GlobalAlloc(dword size, bool user);
__DRVFUNC void   FreezeBlock(Handle h, dword flags);

// What about mapping physical memory to kernel space?
// to an arbitrary location?

void InitMem(dword mem1k_after_1m,
dword mem_after_memhole)
{
}
