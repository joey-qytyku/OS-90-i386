/**
 * @file Resource.c
 * @author Joey Qytyku
 * @brief Resource management for PC/AT
 * @version 0.1
 * @date 2022-06-25
 * 
 * @copyright Copyright (c) 2022
 * @section DESCRIPTION
 * Provides functions for drivers to claim interrupts, DMA, IO,
 * and memory addresses as part of a bus or single driver
 *
 */

#include <DriverLib/Drivers.h>
#include <Platform/Resource.h>
#include <Platform/8259.h>      /* Changing interrupt masks */
#include <Platform/IO.h>        /* Hardware detection */
#include <Type.h>
#include <Linker.h> // To get address of BDA

const char *KERNEL_OWNER = "KERNL386.EXE";

// Interrupt levels of each IRQ

static DRVMUT Interrupt interrupts[16] =
{ // TODO: ADD OWNERS
    [0] =   {STANDARD_32},  // Timer
    [1] =   {STANDARD_32},  // Keyboard
    [2] =   {STANDARD_32},  // Cascade
    [3] =   {UNKNOWN},      // COM, must detect
    [4] =   {UNKNOWN},      // Also COM
    [5] =   {UNKNOWN},      // LPT2 if present
    [6] =   {STANDARD_32},  // Floppy disk controller
    [7] =   {UNKNOWN},      // LPT1
    [8] =   {STANDARD_32},  // RTC
    [9] =   {UNKNOWN},
    [10]=   {UNKNOWN},
    [11]=   {UNKNOWN},
    [12]=   {UNKNOWN},      // PS/2 mouse, may not exist
    [13]=   {STANDARD_32},  // Math coprocessor, HARDWIRED*
    [14]=   {STANDARD_32},  // ATA primary
    [15]=   {STANDARD_32}   // ATA secondary (may not be present?)
};

static DRVMUT word ints_masked;

// *
// The i486DX and better all have integrated floating point
// units, but they also have the FERR# pin, which is used
// to report floating point errors from IRQ#13 for compatibility
// with 80387 code that used this. The IRQ is unavailable for
// any other purposes
//============================================================
// The coprocessor slot on i486 boards is simply another socket
// for an i487, which is a CPU that disables the FPU-less
// package and takes control of the system instead for i486SX PCs
//

//
// A flat list of a memory and IO space usage.
// Entries included are supposed to work on
// nearly all configurations.
//
// ADD SUPPORT FOT PLUG AND PLAY BIOS AUTOCONFIGURATION!!!!
static DRVMUT dword       cur_iorsc = 0; // UPDATE
static DRVMUT IO_Resource resources[MAX_IO_RSC] =
{
    {// Master PIC
        .start = 0x20,
        .limit = 0x21,
        .info = PORT | STD | INUSE,
        .owner = KERNEL_OWNER
    },
    {// 8042 mouse/keyboard controller
        .start = 0x60,
        .limit = 0x64,
        .info = PORT | STD | INUSE,
        .owner = KERNEL_OWNER
    },
    {// Slave PIC
        .start = 0xA0,
        .limit = 0xA1,
        .info = PORT | STD | INUSE,
        .owner = KERNEL_OWNER
    },
    {// Floppy disk controller
        .start = 0x3F0,
        .limit = 0x3F5,
        .info = PORT | STD | INUSE,
        .owner = KERNEL_OWNER
    },

    // GFX/VGA adapter IO ports
    {
        .start = 0x3B0,
        .limit = 0x3DF,
        .info  = PORT | STD | INUSE,
        .owner = KERNEL_OWNER
    },

    {// GFX/VGA video memory
        .start = 0xA0000,
        .limit = 0xBFFFF,
        .info = MEM | STD | INUSE,
        .owner = KERNEL_OWNER
    },

    {// ROM and video BIOS shadow
        .start = 0xC0000,
        .limit = 0xFFFFF,
        .info = MEM | STD | INUSE,
        .owner = KERNEL_OWNER
    },

    // Primary ATA controller
    {
        .start = 0x1F0,
        .limit = 0x1F7,
        .info = PORT | STD | INUSE,
        .owner = KERNEL_OWNER
    },
    // Secondary ATA or same as primary if not present
    {
        .start = 0x170,
        .limit = 0x177,
        .info  = PORT | STD | INUSE
        .owner = KERNEL_OWNER
    },

    {// Primary ATA control port
        .start = 0x3F6,
        .limit = 0x3F7,
        .info = PORT | STD | INUSE,
        .owner = KERNEL_OWNER
    },

    {// Secondary control port
        .start = 0x376,
        .limit = 0x377,
        .info = PORT | STD | INUSE,
        .owner = KERNEL_OWNER
    },
    {// Programmable option select, PS/2, never used
        .start = 0x90,
        .limit = 0x9F,
        .info = PORT | STD | INUSE,
        .owner = KERNEL_OWNER
    },
    {// External floating point unit
        .start = 0xF0,
        .limit = 0xFF,
        .info = PORT | STD | INUSE,
        .owner = KERNEL_OWNER
    },
    { // LED info panel (PS/2)
        .start = 0x108,
        .limit = 0x10F,
        .info  = PORT | STD | INUSE,
        .owner = KERNEL_OWNER
    },
    {// Game port, provided by some sound cards
        .start = 0x200,
        .limit = 0x20F,
        .info  = PORT | STD | INUSE,
        .owner = KERNEL_OWNER
    },
    // No COM or LPT, they are figured out later
    {// Extended NMI
        .start = 0x461,
        .limit = 0x465,
        .info  = PORT | STD | INUSE,
        .owner = KERNEL_OWNER
    }
};

void FindConflict(void)
{
}

/// @brief add a new port/memory mapped resource entry
/// Can be used for a device which expects static assignments
/// or for a PnP bus driver with autoconf disabled for compatibility
///
__DRVFUNC Status AddIOMemRsc(PIO_Resource new_rsc)
{
    if (cur_iorsc >= MAX_IO_RSC)
        return -1;
    C_memcpy(&resources[cur_iorsc], new_rsc, sizeof(IO_Resource));
    cur_iorsc++;
}

/** UNFINISHED!!!!!!!!!!!!!!!!
 * @brief Acquires a range of unused port-mapped IO
 * and gives to a bus driver. Resources are not deallocated
 * so there is no issue of fragmentation, just finds a space that fits
 * @param size Bytes to allocate
 **/
__DRVFUNC Status Bus_AllocateIO(word size, byte align)
{
    dword size_of_portspace;
    dword current_address;
    int i;
    for (int i = 0; i<cur_iorsc-1; i++)
    { // Plus 1?
        size_of_portspace = resources[i].limit - resources[cur_iorsc].start + 1;

    }
    return 0;
}

//============================================================================
// Plug and play BIOS support
//============================================================================

#define PNP_ROM_STRING BYTESWAP(0x24506e50) // "$PnP"

static FarPointer32 pnp16_pmode_fptr;

// Scan the ROM space for "$PnP" at a 2K boundary
void SetupPnP()
{
    const volatile // ROM space should not be prefetched or modified
    pdword rom = (pdword)0xF0000;
    int i;

    for (i=0; i<65536/0x800;i++)
    {
        if (*rom = PNP_ROM_STRING)
            goto hasPnP;
    }
    hasPnP:
    // Get the entry point
}

//
// Call wrapper for PnP calls
//
void CallBiosPnP()
{}


//
// ADD RESOURCE ENTRIES FOR MOTHERBOARD DEVICES
// This gets the device nodes from the PnP BIOS and adds
// the information to the resource. The kernel does not
// recognize devices, only resources.
//
static void PlugAndPlayCfg()
{}

//============================================================================
// Interrupt Management Functions
//============================================================================

/**
 * @brief Get the Int Info object, not for drivers
 * @param v IRQ number
 * @section Arguments
 * dword to avoid unnecessary sign extention
 * @return A pointer to the interrupt, 4-bit normalized
 */
PInterrupt FastGetIntInfo(dword v)
{
    return &interrupts[v & 0xF];
}

/**
 * @brief Assign a handler to an interrupt.
 * This function assigns a single high-level ISR to
 * one or more IRQs. It modifies interrupt structures
 * to point to the ISR and mark it as fast if selected.
 * 
 * Standard interrupts can be modified but only if they are
 * not found to be active.
 * @param bmp_intlines Bitmap of IRQs to take control of
 * @param handler      Pointer to the handler
 * @param fast         If 1, IF is turned off on entry 
 * @param name         String to identify, any langth, must be in memory
 * @return Status
 */
__DRVFUNC Status IntrRequestFixed(word bmp_intlines, PHandler handler, bool fast, char* name)
{
    int irq;
    for (irq=0; irq < 16; irq++)
    {
        if ((bmp_intlines >> bit) & 1)
        {
            const Interrupt *theint = interrupts[irq];

            IntsOff();
            switch (theint->intlevel)
            {
            case TAKEN_32:
                return RQINT_FAILED;
            break;

            case STANDARD_32:
            case RECL_16:
                theint->owner = name;
                theint->fast = fast;
                theint->handler = handler;
                theint->intlevel= TAKEN_32;
                return RQINT_TAKEN;
            break;
            default:
                // Error with hardware detection probably
                return RQINT_FAILED
            }
        }
    }
    return 0;
}

/**
 * @brief   Unmask the interrupts specified, modifies the interrupt array
 *          so that it says enabled and that the state of the IRQ can be
 *          found without having to read the IMR
 * @param bmp_intlines A bitmap of the interrupt lines to activate
 * @return Notification
 * @retval RQINT_INUSE32  In use by another driver
 * @retval RQINT_STANDARD This interrupt is stanard hardware
 * @retval RQINT_TAKEN 0  Taken without any issues
*/
__DRVFUNC void IntrEnable(word bmp_intlines)
{
    ints_masked |= bmp_intlines;
}

void InitResMGR(bool fpu_present, bool fpu_internal)
{
    cur_iorsc = 20;
}


