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
static IMUSTR description = "Kernel plug-and-play support";

////////////////////////////////////////////////////////////////////////////////
// The kernel is a bus and has access to all resources on the system
// It handles regular PnP functionality with system board devices
// (obviously not remove/insert events)
//
DRIVER_HEADER kernel_bus_hdr =
{
    .driver_name   = &driver_name,
    .description   = &description,
    .cmdline       = NULL,
    .driver_flags  = DF_BUS,
    .event_handler = NULL,
    .next_driver   = NULL
};

//
// Non-standard IRQs are FREE but if they are found
// to have been modified by a DOS program they
// are set to RECL_16
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
static MCHUNX IO_RESOURCE resources[MAX_IO_RSC];
static MCHUNX WORD        mask_bitmap = 0xFFFF;
static MCHUNX INTERRUPTS  interrupts = { 0 };

/*
Supported operations with interrupts:

Set operations:
* Request from bus
* Request from kernel bus (legacy)
* Surrender to DOS (change to RECL_16)
* Reclaim from DOS (change to INUSE)

Get operations:
* Get 32-bit handler address
*

*/

////////////////////////////////////////////////////////////////////////////////
// Brief:
//  Get the Int Info object, not for drivers
// v:
//  IRQ number
// DWORD to avoid unnecessary sign extention
// Return: A pointer to the interrupt, 4-bit normalized
//
static VOID SetInterruptEntry(
    VINT            irq,
    INTERRUPT_LEVEL lvl,
    FP_IRQ_HANDLR   handler,
    PDRIVER_HEADER  owner
){

    EnterCriticalSecttion();

    interrupts.handlers[irq] = handler;
    interrupts.owners[irq] = owner;
    interrupts.lvl_bmp |= lvl << (irq * 2);

    ExitCriticalSection();
}

//
// A driver or the kernel can voluntarily give an interrupt back to DOS
// Potentially to unload.
//
VOID InSurrenderInterrupt()
{}

INTERRUPT_LEVEL InGetInterruptLevel(VINT irq)
{
    return interrupts.lvl_bmp >>= irq * 2;
}

//
// Get the address of the handler
//
FP_IRQ_HANDLR InGetInterruptHandler(VINT irq)
{
    return interrupts.handlers[irq];
}

// Brief:
//  Legacy IRQ means it the exact IRQ is known by the
//  user and the driver. This function will add a handler
//  and set the interrupt to BUS_INUSE
//
STATUS InAcquireLegacyIRQ(VINT fixed_irq,
                          FP_IRQ_HANDLR handler)
{
    SetInterruptEntry(
        fixed_irq,
        BUS_INUSE,
        handler,
        &kernel_bus_hdr
    );
}

//
// This API call will reqest the bus driver to give this IRQ to the client.
// Requests to own a bus interrupt are always routed to the particular driver
// for it to handle it how it wants to.
//
STATUS APICALL InRequestBusIRQ(PDRIVER_HEADER bus,
                               PDRIVER_HEADER client,
                               VINT vi,
                               FP_IRQ_HANDLR handler)
{
}

//
// Note to self: How will I implement requesting the IRQ based on device ID?
//

////////////////////////////////////////////////////////////////////////////////
// Plug and Play Bios communication support, refferences code in PnP_Mgr.asm
////////////////////////////////////////////////////////////////////////////////

VOID APICALL PnBiosCall()
{
}


// Scan the ROM space for "$PnP" at a 2K boundary
STATUS SetupPnP(VOID)
{
    // ROM space should not be prefetched or written
    volatile PPNP_INSTALL_CHECK checkstruct = (PPNP_INSTALL_CHECK)0xF0000;
    BOOL supports_pnp;
    BYTE compute_checksum;

    // Find the checkstruct
    for (DWORD i = 0; i<0x800*32; i++)
    {
        if (*(PPNP_INSTALL_CHECK)checkstruct->signature == PNP_ROM_STRING)
        {
            has_pnp=1;
            break;
        }
        checkstruct += 0x800; //????
    }
    if (!has_pnp)
        return OS_FEATURE_NOT_SUPPORTED;

    PnSetBiosDsegBase(checkstruct->protected_data_base);
    PnSetBiosCsegBase(checkstruct->protected_base);

    return OS_OK;
}
////////////////////////////////////////////////////////////////////////////////
// Plug and Play kernel event handling and sending
////////////////////////////////////////////////////////////////////////////////
VOID PnSendDriverEvent()
{
    // Should this require sender != to reciever?
}
//Kernel does not handle events !!!!!!!!!!!!!!! Where to put this?
//
STATUS KernelEventHandler(PDRIVER_EVENT_PACKET)
{
    return OS_FEATURE_NOT_SUPPORTED;
}

////////////////////////////////////////////////////////////////////////////////
// IO and Memory Resource Managment Reoutines
////////////////////////////////////////////////////////////////////////////////

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
    PDWORD saved_master_v = phys(655360-1024-64),
           saved_slave_v  = phys(655360-1024-64+8);
    PDWORD master_v = phys(32), slave_v = phys(0x70*4);
    BYTE i;

    // I should optimize these loops, this is kind of bad
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
    if (GetInterruptLevel(2) == RECL_16)
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
    for (i = 0; i < 4; i++) // Up to four COM ports on an IBM PC
    {
        if (bda[i] != 0) // then COM[i] exists
            com_ports++;
    }
    // RECL_16 is used because a 32-bit driver for COM is not loaded yet
//    interrupts[4].lvl = RECL_16;


    if (com_ports > 1);
//        interrupts[3].lvl = RECL_16;
}

VOID InitPnP()
{
}
