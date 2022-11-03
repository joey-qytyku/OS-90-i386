/*
     This file is part of OS/90.

    OS/90 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

    OS/90 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with OS/90. If not, see <ttps://www.gnu.org/licenses/>.
*/


#include <PnP_Mgr.h>
#include <Linker.h>
#include <Atomic.h>
#include <Type.h>

#define PNP_ROM_STRING BYTESWAP(0x24506e50) /* "$PnP" */
#define NUM_INT 16 /* For consistency */

static IMUSTR driver_name = "Kernl386.exe";
static IMUSTR description = "Kernel plug-and-play support"

STATUS KernelEventHandler(PDRIVER_EVENT_PACKET dep)
{
    return OS_FEATURE_NOT_SUPPORTED;
}

////////////////////////////////////////////////////////////////////////////////
// The kernel is a bus and has access to all resources on the system
// It handles regular PnP functionality with system board devices
// (obviously not remove/insert events)
//
DRIVER_HEADER kernel_bus_hdr =
{
    .driver_name = &driver_name,
    .description = &description,
    .cmdline     = NULL,
    .driver_flags = DF_BUS,
    .event_handler = NULL,
    .next_driver = NULL
}

//
// Non-standard IRQs are FREE but if they are found
// to have been modified by a DOS program they
// are set to RECL_16
//

//
// DO INTERRUPTS HAVE TO BE VOLATILE?
//

////////////////////////////////////////////////////////////////////////////////
// PnP manager functions that modify the interpretation
// of interrupts may not run inside an ISR.
//
// Despite the fact that other driver can modify resource
// information indirectly with function calls, there is no
// need for volatile because that would not be unexpected and
// kernel code cannot be interrupted by a process or driver
////////////////////////////////////////////////////////////////////////////////

static MCHUNX DWORD       cur_iorsc = 0;
static IO_RESOURCE resources[MAX_IO_RSC];
static INTERRUPT   interrupts[NUM_INT] = {0 /* All are free */ };
static WORD        mask_bitmap = 0xFFFF;
//
// Kernel does not handle events
//

////////////////////////////////////////////////////////////////////////////////
// Brief:
//  Get the Int Info object, not for drivers
// v:
//  IRQ number
// DWORD to avoid unnecessary sign extention
// Return: A pointer to the interrupt, 4-bit normalized
//
PINTERRUPT InFastGetInfo(VINT i)
{
    return &interrupts[i];
}

static VOID SetInterruptEntry(
    VINT            irq,
    INTERRUPT_LEVEL lvl,
    PIRQ_HANDLR     handler,
    PDRIVER_HEADER  owner
){
    PINTERRUPT i = &interrupts[irq];

    EnterCriticalSecttion();

    i->owner     = owner;
    i->handler   = handler;
    i->lvl  = lvl;

    ExitCriticalSection();
}

VOID APICALL PnBiosCall()
{
}

//
//
//
STATUS KernelEventHandler(PDRIVER_EVENT_PACKET)
{
    return OS_FEATURE_NOT_SUPPORTED;
}


// Scan the ROM space for "$PnP" at a 2K boundary
STATUS SetupPnP(VOID)
{
    // ROM space should not be prefetched or written
    const volatile PPNP_INSTALL_CHECK checkstruct = (PPNP_INSTALL_CHECK)0xF0000;
    DWORD i;
    BYTE compute_checksum;

    for (i=0; i<65536/0x800;i++)
    {
        if (checkstruct->signature == PNP_ROM_STRING)
        {
            for (i=0; i<21; i++)
                compute_checksum += *(PBYTE)rom + i;

            if (compute_checksum == 0)
                goto HasPnP;
            else
                return OS_FEATURE_NOT_SUPPORTED;
        }
    }
    HasPnP:
    PnSetBiosDsegBase(checkstruct->protected_data_base);
    PnSetBiosCsegBase(checkstruct->protected_base);

    return OS_OK;
}

// Brief:
//  Legacy IRQ means it the exact IRQ is known by the
//  user and the driver. This function will add a handler
//  and set the interrupt to BUS_INUSE
//
STATUS InAcquireLegacyIRQ(VINT fixed_irq, PIRQ_HANDLR handler)
{
    SetInterruptEntry(
        fixed_irq,
        BUS_INUSE,
        handler,
        &kernel_bus_hdr
    );
}

VOID PnSendDriverEvent()
{
    // Should this require sender != to reciever?
}

//
//
STATUS APICALL InRequestBusIRQ(PDRIVER_HEADER driver, VINT vi)
{
}

// Add a new port/memory mapped resource entry
STATUS APICALL PnAddIOMemRsc(PIO_RESOURCE new_rsc)
{
    if (cur_iorsc >= MAX_IO_RSC)
        return -1;
    resources[cur_iorsc] = *new_rsc;
    cur_iorsc++;
    return 0;
}

STATUS APICALL Bus_AllocateIO(WORD size, BYTE align)
{
}

// Brief:
//  Detect which interrupts are unused by non-PnP devices
// Explaination:
//  A program called GRABIRQ.SYS saves the default assignments
//  and compares them after DOS is initialized.
// Note: <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//  If IRQ#2 is modified for some reason, an IRQ will go to IRQ#9,
//  so IRQ#9 cannot be used
//
// By default, the BIOS sends IRQ#9 back to IRQ#2 handler so that a
// program designed for the single PIC thinks a real IRQ#2 happened
//
static VOID DetectFreeInt(VOID)
{
    PDWORD saved_master_v = phys(655360-1024-64);
           saved_slave_v  = phys(655360-1024-64+8);
    PDWORD master_v = phys(32), slave_v = phys(0x70*4);
    BYTE i;

    // I should optimize these loops
    for (i=0; i<8; i++)
    {
        if (saved_master_v[i] != master_v[i])
        {
            SetInterruptEntry(
                i,
                RECL_16,
                NULL,
                &kernel_bus_hdr
            );
        }
    }
    for (i=0; i<8; i++)
    {
        if (slave_v[i+8] != saved_slave_v[i])
            SetInterruptEntry(
                i+8,
                RECL_16,
                NULL,
                &kernel_bus_hdr
            );
    }
    if (interrupts[2].lvl == RECL_16)
    { 
        // IRQ#2 was hooked by DOS. What now?
    }
}

// Brief:
//  Detect how many COM ports there are and where they
//  are located by reading BDA. The IO base is usually standard
//  but some BIOSes allow ports to be changed.
//
// This required because interrupt levels need to be configured
// even with resource managment turned off
// Notes:
//  COM2 and COM4 => IRQ#3
//  COM1 and COM3 => IRQ#4
//
static VOID DetectCOM(VOID)
{
    BYTE i, com_ports;
    const PWORD bda = (PWORD)phys(0x400+0);

    // Check BIOS data area for number of serial ports
    // The beginning words are the COM port IO addresses
    // Zero indictates not present
    //
    for (i = 0; i < 4; i++) // Up to four COM ports on PC
    {
        if (bda[i] != 0) // then COM[i] exists
            com_ports++;
    }
    // RECL_16 is used because a 32-bit driver for COM is not loaded yet
    interrupts[4].lvl = RECL_16;
    if (com_ports > 1)
        interrupts[3].lvl = RECL_16;
}

VOID InitPnP()
{
}
