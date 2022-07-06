#ifndef RESOURCE_H
#define RESOURCE_H

#include <DriverLib/Drivers.h>
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

extern const char *KERNEL_OWNER;

enum InterruptLVL
{
    UNKNOWN = 0, /* Temporary, requires HW detection */
    RECL_16,     /* Potential 16-bit driver */
    TAKEN_32,    /* A driver is already using */
    STANDARD_32, /* Standard PC interrupt, cannot be used by a bus */
};
/*
 * Standard interrupts can be modified but already taken one cannot be
*/

typedef struct {
    byte intlevel;
    PHandler handler;
    bool fast;
    char* owner;
}Interrupt,*PInterrupt;

typedef struct {
}COM_Port;

typedef struct
{   dword start;
    dword limit;
    dword info;
}IO_Resource,*PIO_Resource; // IO ports or memory mapped IO

#define RQINT_FAILED -1
#define RQINT_TAKEN   0

__DRVFUNC Status IntrRequestFixed(word, PHandler, bool, char*);
PInterrupt FastGetIntInfo(byte);
#endif /* RESOURCE_H */
