#ifndef RESOURCE_H
#define RESOURCE_H

#include <Type.h>

#define MAX_STD_RSC 64
#define MAX_IO_RSC 64

#define MEM 0
#define AVAIL 0

#define PORT 1
#define STD 2
#define INUSE 4
#define CACHABLE 8

typedef int (*PHandler)(void*);

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
    char owner[16]; // FAT case w/out extension, zero terminated
}Interrupt,*PInterrupt;

typedef struct {
}COM_Port;

typedef struct
{   dword start;
    dword limit;
    dword info;
}IO_Resource; // IO ports or memory mapped IO

__DRVFUNC int RequestIntLines(byte, PHandler, pchar);

#define RQINT_TAKEN 1

#endif /* RESOURCE_H */
