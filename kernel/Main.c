#include <Type.h>
#include <IA32.h>

struct info {}__PACKED;

void KernelMain(pvoid info)
{
    InitIA32();
}
