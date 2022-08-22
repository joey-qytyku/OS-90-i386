/*
     This file is part of OS/90.

    OS/90 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

    OS/90 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with OS/90. If not, see <https://www.gnu.org/licenses/>. 
*/

#include <Platform/Resource.h>
#include <Platform/IA32.h>    /* Long jump? */
#include <Type.h>

#define PNP_ROM_STRING BYTESWAP(0x24506e50) /* "$PnP" */
#define NUM_INT 16 /* For consistency */

IMUSTR KERNEL_OWNER = "KERNL386.EXE";

//
// A flat list of a memory and IO space usage.
// Entries included are supposed to work on
// nearly all configurations.
//

static MCHUNX DWORD cur_iorsc = 0;

// This is empty because if there is no PnP support
// resource management is determined by the
// configuration of the PC, otherwise, PnP BIOS reports all
// motherboard devices
static MCHUNX IO_RESOURCE resources[MAX_IO_RSC];

// Non-standard IRQs are FREE but if they are found
// to have been modified by a DOS program they
// are set to RECL_16
static MCHUNX INTERRUPT interrupts[NUM_INT] =
{ // TODO: ADD OWNERS
    [0] =   {STANDARD_32},  // Timer
    [1] =   {STANDARD_32},  // Keyboard
    [2] =   {STANDARD_32},  // Cascade
    [3] =   {RECL_16},      // COM, certainly exists
    [4] =   {UNKNOWN},      // Also COM
    [5] =   {UNKNOWN},      // LPT2 if present
    [6] =   {STANDARD_32},  // Floppy disk controller
    [7] =   {UNKNOWN},      // LPT1
    [8] =   {STANDARD_32},  // CMOS/RTC
    [9] =   {FREE},
    [10]=   {FREE},
    [11]=   {FREE},
    [12]=   {FREE},         // PS/2 mouse, may not exist (Compaq Deskpro?)
    [13]=   {STANDARD_32},  // Math coprocessor, HARDWIRED*
    [14]=   {STANDARD_32},  // ATA primary
    [15]=   {STANDARD_32}   // ATA secondary (may not be present?)
};

static DRVMUT word ints_masked;

// Brief:
//  Get the Int Info object, not for drivers
// v:
//  IRQ number
// dword to avoid unnecessary sign extention
// Return: A pointer to the interrupt, 4-bit normalized
//
PINTERRUPT FastGetIntInfo(DWORD v)
{
    return &interrupts[v & 0xF];
}

// Brief:
// Assign a handler to an interrupt.
// This function assigns a single high-level ISR to
// one or more IRQs. It modifies interrupt structures
// to point to the ISR and mark it as fast if selected.
// 
// Standard interrupts can be modified but only if they are
// not found to be active.
// bmp_intlines Bitmap of IRQs to take control of
// handler      Pointer to the handler
// fast         If 1, IF is turned off on entry 
// name         String to identify, any langth, must be in memory
// Return Status
//
STATUS APICALL InRequestFixed(
    WORD     bmp_intlines,
    PHANDLER handler,
    BOOL     fast,
    PIMUSTR  name
){
    DWORD irq;
    for (irq=0; irq < NUM_INT; irq++)
    {
        if (BIT_IS_SET(bmp_intlines, irq)) // Make this faster?
        {
            PINTERRUPT theint = &interrupts[irq];

            IntsOff();
            switch (theint->intlevel)
            {
            case TAKEN_32:
                return OS_ERROR_GENERIC;
            break;

            case STANDARD_32:
            case RECL_16:
                theint->owner = name;
                theint->fast = fast;
                theint->handler = handler;
                theint->intlevel= TAKEN_32;
                return OS_OK;
            break;
            default:
                // Error with hardware detection probably
                return OS_ERROR_GENERIC;
            }
        }
    }
    return OS_OK;
}

void APICALL PnBiosCall()
{
}

// Brief:
//  Unmask the interrupts specified, the state of IRQ can be
//  found without having to read the IMR
// bmp_intlines: A bitmap of the interrupt lines to activate
//
void APICALL IntrEnable(word bmp_intlines)
{
    ints_masked |= bmp_intlines;
}

//*The i486DX and better all have integrated floating point
//*units, but they also have the FERR# pin, which is used
//*to report floating point errors from IRQ#13 for compatibility
//*with 80387 code that used this. The IRQ is unavailable for
//*any other purposes
//============================================================
//*The coprocessor slot on i486 boards is simply another socket
//*for an i487, which is a CPU that disables the FPU-less
//*package and takes control of the system instead for i486SX PCs

// Scan the ROM space for "$PnP" at a 2K boundary
STATUS SetupPnP(void)
{
    // ROM space should not be prefetched or written
    const volatile PPNP_INSTALL_CHECK checkstruct = (pdword)0xF0000;
    dword i;
    byte compute_checksum;

    for (i=0; i<65536/0x800;i++)
    {
        if (checkstruct->signature == PNP_ROM_STRING)
        {
            for (i=0; i<21; i++)
                compute_checksum += *(pbyte)rom + i;

            if (compute_checksum == 0)
                goto HasPnP;
            else
                return OS_FEATURE_NOT_SUPPORTED;
        }
    }
    HasPnP:

    // The GDT must be updated so that the PnP code segment
    // is pointed to by the entry

    // PnP requires a 16-bit data and code segment
    // but the BIOS is required to support 32-bit stacks

    return OS_OK;
}

void PnFindConflict(void)
{
    // Make sure that memory holes do not touch the kernel.
    // Allocator is an object/struct with base address?
}

// Add a new port/memory mapped resource entry
APICALL STATUS PnAddIOMemRsc(PIO_Resource new_rsc)
{
    if (cur_iorsc >= MAX_IO_RSC)
        return -1;
    C_memcpy(&resources[cur_iorsc], new_rsc, sizeof(IO_Resource));
    cur_iorsc++;
    return 0;
}

// UNFINISHED!!!!!!!!!!!!!!!!
// @brief Acquires a range of unused port-mapped IO
// and gives to a bus driver. Resources are not deallocated
// so there is no issue of fragmentation, just finds a space that fits
// @param size Bytes to allocate
//
APICALL STATUS Bus_AllocateIO(word size, byte align)
{
    dword size_of_portspace;
    dword current_address;
    int i;

    for (int i = 0; i<cur_iorsc-1; i++)
    {   // Plus 1?
        size_of_portspace = resources[i].limit - resources[cur_iorsc].start + 1;
    }
    return 0;
}

// Brief:
//  Detect which interrupts are unused by non-PnP devices
// Explaination:
//  A program called GRABIRQ.SYS saves the default assignments
//  and compares them after DOS is initialized.
//
static void DetectFreeInt(void)
{
    pdword saved_master_v = phys(655360-1024-64);
           saved_slave_v  = phys(655360-1024-64+8)
    pdword master_v = phys(32), slave_v = phys(0x70*4);
    byte i;

    // I should optimize these loops
    for (i=0; i<8; i++)
    {
        if (saved_master_v[i] != master_v[i])
            interrupts[i].intlevel = RECL_16;
    }
    for (i=0; i<8; i++)
    {
        if (slave_v[i+8] != saved_slave_v[i])
            interrupts[i+8].intlevel = RECL_16;;
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
static void DetectCOM(void)
{
    byte i, com_ports;
    const pword bda = (pword)phys(0x400+0);

    // Check BIOS data area for number of serial ports
    // The beginning words are the COM port IO addresses
    // Zero indictates not present
    //
    for (i = 0; i < 4; i++) // Up to four COM ports on PC
    {
        if (bda[i] != 0) // then COM[i] exists
        {
            IO_Resource new = {
                .start = bda[i],
                .limit = bda[i]+7,
                .info  = INUSE | STD | PORT
            };
            PnAddIOMemRsc(&new);
            com_ports++;
        }
    }
    // RECL_16 is used because a 32-bit driver for COM is not loaded yet
    interrupts[4].intlevel = RECL_16;
    if (com_ports > 1)
        interrupts[3].intlevel = RECL_16;
}

void InitPnP(byte fpu_status)
{
}
