/* Memory manager */

#include <Type.h>
#include <Linker.h>

static void *alloc_base = NULL;

static dword AddrAlign(void *addr, dword bound)
{
    return ((dword)addr + bound - 1) & ~(bound-1);
}

void InitMem()
{
}
