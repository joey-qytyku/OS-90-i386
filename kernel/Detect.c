#include <Platform/Resource.h>
#include <Type.h>

// Should memory allocation be tied to resource management?

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
                .info  = INUSE | STD | PORT,
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
