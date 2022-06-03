/*
 * For kernel use only
 */

#include <Type.h>

typedef struct __attribute__((packed)) {
    dword   eax,ebx,ecx,edx,esp,ebp;
    dword   ss,cs,eip,eflags; // segments zero extended here
}reg86;

extern void EnterV86(reg86*);
