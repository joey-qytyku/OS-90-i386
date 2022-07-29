#ifndef PNP_MGR_H
#define PNP_MGR_H

#include <DriverLib/Drivers.h>
#include <Type.h>

#define MAX_IO_RSC 64

#define MEM 0
#define AVAIL 0

#define PORT 1
#define STD 2
#define INUSE 4
#define MEM_CACHABLE 8

#define OWNER_NAME_SIZE 16

#define PNP_FPU_PRESENT  0x01
#define PNP_FPU_INTERNAL 0x02

#define RQINT_FAILED -1
#define RQINT_TAKEN   0

typedef int (*PHandler)(void*);

extern const char *KERNEL_OWNER;

enum InterruptLVL
{
    UNKNOWN = 0, // Temporary, requires HW detection
    FREE,        // Free to use
    RECL_16,     // 16-bit driver
    TAKEN_32,    // A driver is already using
    STANDARD_32, // Standard PC interrupt, cannot be used by a bus
};
/*
 * Standard interrupts can be modified but already taken one cannot be
*/

typedef struct {
    byte intlevel;
    PHandler handler;
    bool fast;
    byte pnp_vendor;
    word pnp_id;
    char* owner;
}Interrupt,*PInterrupt;

typedef struct
{   dword start;
    dword limit;
    dword info;
    char *owner;
}IO_Resource,*PIO_Resource; // IO ports or memory mapped IO

__DRVFUNC
Status InRequestFixed(word, PHandler, bool, char*);

__DRVFUNC
Status PnAddIOMemRsc(PIO_Resource);

#ifndef __PROGRAM_IS_DRIVER

PInterrupt InFastGetIntInfo(dword);
void InitPnP(byte)

#endif /* __PROGRAM_IS_DRIVER */

#endif /* PNP_MGR_H */