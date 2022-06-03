#include <Type.h>
#include <IA32.h>

void KernelMain(void *info)
{
    InitIA32();
    EarlyInitPIC();
}
