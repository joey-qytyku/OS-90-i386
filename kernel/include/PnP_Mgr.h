/*
     This file is part of OS/90.

    OS/90 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

    OS/90 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with OS/90. If not, see <ttps://www.gnu.org/licenses/>.
*/

#ifndef PNP_MGR_H
#define PNP_MGR_H

#include <DriverLib/Drivers.h>
#include <Platform/IA32.h>
#include <Type.h>

#define MAX_IO_RSC 64

#define MEM 0
#define AVAIL 0

// Resource flags byte:
//|AA|DD|C|U|S|P|
// P=PORT
// S=STD
// U=INUSE
// C=MEM_CACHEABLE
// DD=
// Uses bit fields instead of byte

#define PORT 1
#define STD 2
#define INUSE 4
#define MEM_CACHABLE 8

#define IOP_DECODE_10 0
#define IOP_DECODE_16 1
#define MEM_DECODE_24 2
#define MEM_DECODE_32 3

#define ACCESS_8  0
#define ACCESS_16 1
#define ACCESS_32 2

#define DF_BUS 1

//#define PNP_FPU_PRESENT  0x01
//#define PNP_FPU_INTERNAL 0x02

typedef STATUS (*PEVENT_HANDLER)(PVOID);
typedef VOID   (*PIRQ_HANDLR)   (PREGS_IA32);
typedef BYTE RESOURCE_INF;

typedef DWORD VINT;

typedef enum {
    UNDEFINED = 0,
    BUS_FREE  = 1,
    BUS_INUSE = 2,
    RECL_16   = 3
}INTERRUPT_LEVEL;

typedef enum {
    MJ_PNP_UNLOAD,
    MJ_PNP_LOAD,
    MJ_PNP_DISABLE,
    MJ_PNP_ENABLE
}PNP_EVENT_CODE;

typedef struct
{
    WORD    major;
    WORD    minor;

}DRIVER_EVENT_PACKET,
*PDRIVER_EVENT_PACKET;

typedef struct {
    PIMUSTR     driver_name;
    PIMUSTR     description;
    PIMUSTR     cmdline;
    DWORD       driver_flags;
    PVOID       next_driver;
    PEVENT_HANDLER event_handler;
}DRIVER_HEADER,*PDRIVER_HEADER;

typedef struct {
    INTERRUPT_LEVEL lvl;
    PIRQ_HANDLR     handler;
    PDRIVER_HEADER  owner;
}INTERRUPT,
*PINTERRUPT;

// Replace with this

typedef struct __attribute__((packed)){
    char    lvl:2;
    long   handlers;
    long   owners;
}INTERRUPT,
*PINTERRUPT;


/*
The ISA bus supports 24-bit addresses and up to 16-bit IO ports
Many devices use 10-bit decode.

This is really bad because any address with the bottom bits
reffering to the legacy device will access it, even if the top
bits are something else
*/

//
// owner_krel is a 24-bit offset to C0000000. For bit packing.
// This does not matter to any of the drivers.
//
typedef struct __attribute__((packed))
{
    DWORD          start;
    DWORD          size:24;
    DWORD          alignment:24;
    PVOID          owner_krel;
    BYTE
        is_port:1,
        is_std:1,
        inuse:1,
        mem_cachable:1,
        io_decode:2,
        io_access:2;
}IO_RESOURCE,*PIO_RESOURCE;  // IO ports or memory mapped IO


//
// PIO_RESOURCE is MCHUNX because variable interrupts is MCHUNX
//

//
// An INTERRUPT type is not MCHUNX unless specified, but a pointer to one must
// be because it will certainly point to a MCHUNX INTERRUPT object
//

typedef struct {
    DWORD   signature;
    BYTE    version;
    BYTE    length;
    WORD    control_field;
    BYTE    checksum;
    PVOID   event_notification;
    WORD    _real_mode_code_off;
    WORD    _real_mode_code_seg;
    WORD    protected_off;
    DWORD   protected_base;
    DWORD   oem_device_id;
    WORD    _real_mode_data_seg;
    WORD    protected_data_base;
}*PPNP_INSTALL_CHECK;

STATUS APICALL PnAddIOMemRsc(PIO_RESOURCE);

#ifndef __PROGRAM_IS_DRIVER

PINTERRUPT InFastGetInfo(VINT i);
VOID InitPnP(BYTE)

__attribute__(( regparm(1) ))
WORD PnCallBiosInternal(DWORD argc, ...);


#endif /* __PROGRAM_IS_DRIVER */

#endif /* PNP_MGR_H */