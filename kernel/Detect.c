#include <Platform/Resource.h>
#include <Platform/IA32.h>    /* Long jump */
#include <Type.h>

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
    hasPnP:;
    // Get the protected mode entry point
    // PnP requires a 16-bit data and code segment
    // but the firmware is required to support 32-bit stacks
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
// Hardware detection functions, only used if there is no PnP support
//============================================================================

#ifdef KERNEL_NO_PNP_SUPPORT

// Make sure that memory holes do not touch the kernel.

// Allocator is an object/struct with base address?

static void DetectBus()
{
}

/// @brief Detect which interrupt are unused by ISA devices
/// @section Explaination
/// Windows 9x has a special IO.SYS that detects modifications
/// to interrupt vectors so that it can call 16-bit drivers.
/// A program called GRABIRQ.SYS records modifications to IRQ
/// vectors by hooking the DOS interrupts for that purpose.
///
static void DetectFreeInt(void)
{
}

static void DetectPs2Mouse(void)
{
    //
}

/// @brief Detect how many COM ports there are and where they
/// are located by reading BDA. The IO base is usually standard
/// but some BIOSes allow ports to be changed.
/// If a computer has more than one serial port
/// @section Notes
/// then IRQ 3 and 4 are both for COM
/// COM2 and COM4 => IRQ#3
/// COM1 and COM3 => IRQ#4
///
static void DetectCOM(void)
{
    byte i, com_ports;
    const pword bda = (pword)phys(0x400+0);

    /// Check BIOS data area for number of serial ports
    /// The beginning words are the COM port IO addresses
    /// Zero indictates not present
    ///
    for (i = 0; i < 4; i++) // Up to four COM ports on PC
    {
        if (bda[i] != 0) // then COM[i] exists
        {
            IO_Resource new = {
                .start = bda[i],
                .limit = bda[i]+7,
                .info  = INUSE | STD | PORT
            };
            AddIOMemRsc(&new);
            com_ports++;
        }
    }
    if (com_ports > 1)
    {
        // If there is only one COM, IRQ#3 may be
        // configured to use something else, perhaps
        // a 16-bit driver

    }
}

#endif /* KERNEL_NO_PNP_SUPPORT */
