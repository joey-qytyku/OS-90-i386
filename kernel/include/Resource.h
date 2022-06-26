#ifndef RESOURCE_H
#define RESOURCE_H

#include <lib/Drivers.h>
#include <Type.h>

#define RESRV_RSC 20
#define MAX_IO_RSC 64

#define MEM 0
#define AVAIL 0

#define PORT 1
#define STD 2
#define INUSE 4
#define CACHABLE 8

#define OWNER_NAME_SIZE 16

typedef int (*PHandler)(void*);

extern const psbyte KERNEL_OWNER;

enum InterruptLVL
{
    UNKNOWN = 0, /* Temporary, requires HW detection */
    RECL_16,     /* Potential 16-bit driver */
    TAKEN_32,    /* A driver is already using */
    STANDARD_32, /* Standard PC interrupt, cannot be used by a bus */
};

typedef struct {
    byte intlevel;
    bool fast;
    bool enabled;
    PHandler handler;
    char owner[OWNER_NAME_SIZE];
}Interrupt,*PInterrupt;

typedef struct {
}COM_Port;

typedef struct
{   dword start;
    dword limit;
    dword info;
}IO_Resource,*PIO_Resource; // IO ports or memory mapped IO

#define RQINT_TAKEN 1

__DRVFUNC Status RequestFixedLines(byte,PHandler,psbyte);
__DRVFUNC PInterrupt GetIntInfo(byte );
#endif /* RESOURCE_H */
