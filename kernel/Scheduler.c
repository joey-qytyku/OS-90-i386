#include <Type.h>
#include <vm86.h>

enum    LastContext {USER, KERNEL, INTERRUPT};
extern  dword CurrProcID;

void GPF_NoVM86()
{}

void InitScheduler()
{}
