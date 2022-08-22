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

//#define PNP_FPU_PRESENT  0x01
//#define PNP_FPU_INTERNAL 0x02

typedef int (*PHANDLER)(void*);
typedef WORD PNP_RET;

extern PIMUSTR KERNEL_OWNER;

enum InterruptLVL
{
    UNKNOWN = 0, // Temporary, requires HW detection
    FREE,        // Free to use
    RECL_16,     // 16-bit driver
    TAKEN_32,    // A driver is already using
    STANDARD_32, // Standard PC interrupt, cannot be used by a bus
};

typedef tstruct __PACKED {
    BYTE     intlevel:4;
    BYTE     fast:4;
    PHANDLER handler;
    DWORD    compressed_pnpid;
    PIMUSTR  owner;
}INTERRUPT,*PINTERRUPT;

typedef struct
{   DWORD start;
    DWORD limit;
    DWORD info;
    PIMUSTR owner;
}IO_RESOURCE,*PIO_RESOURCE;  // IO ports or memory mapped IO

typedef struct {
    DWORD   signature;
    BYTE    version;
    BYTE    length;
    WORD    control_field;
    BYTE    checksum;
    PVOID   event_notification;
    WORD    __real_mode_code_off;
    WORD    __real_mode_code_seg;
    WORD    protected_off;
    DWORD   protected_base;
    DWORD   oem_device_id;
    WORD    __real_mode_data_seg;
    WORD    protected_data_base;
}*PPNP_INSTALL_CHECK;

typedef struct {
    DWORD       unique_id;
    PIMUSTR     bus_name;
    PEVENTF     event_handler;
}BUSDRV_INFO;

STATUS APICALL InRequestFixed(WORD, PHANDLER, BOOL, PIMUSTR);
STATUS APICALL PnAddIOMemRsc(PIO_Resource);

#ifndef __PROGRAM_IS_DRIVER

PINTERRUPT InFastGetIntInfo(DWORD);
VOID InitPnP(BYTE)
__attribute__(( regparm(1) )) PNP_RET PnCallBiosInternal(DWORD argc, ...);


#endif /* __PROGRAM_IS_DRIVER */

#endif /* PNP_MGR_H */