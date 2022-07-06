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
`````````   
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

/**
 * A flat list of a memory and IO space usage. Standard IO
 * spaces found on all PC-compatibles are pre-included.
 * 20 entries are reserved (0-19).
 */
static DRVMUT dword       cur_iorsc = 0;
static DRVMUT IO_Resource resources[MAX_IO_RSC] = { // FPU reset?
    {// Master PIC
     .start = 0x20,
     .limit = 0x21,
     .info = PORT | STD | INUSE},

    {// 8042 mouse/keyboard controller
     .start = 0x60,
     .limit = 0x64,
     .info = PORT | STD | INUSE},

    {// Slave PIC
     .start = 0xA0,
     .limit = 0xA1,
     .info = PORT | STD | INUSE},

    {// Floppy disk controller
     .start = 0x3F0,
     .limit = 0x3F5,
     .info = PORT | STD | INUSE},

    // GFX/VGA adapter IO ports
    {
        .start = 0x3B0,
        .limit = 0x3DF,
        .info  = PORT | STD | INUSE
    },

    {// GFX/VGA video memory
        .start = 0xA0000,
        .limit = 0xBFFFF,
        .info = MEM | STD | INUSE},

    {// ROM and video BIOS shadow
        .start = 0xC0000,
        .limit = 0xFFFFF,
        .info = MEM | STD | INUSE},

    // Primary ATA controller
    {
        .start = 0x1F0,
        .limit = 0x1F7,
        .info = PORT | STD | INUSE},
    // Secondary ATA or same as primary if not present
    {
        .start = 0x170
        .limit = 0x177},

    {// Primary ATA control port
        .start = 0x3F6,
        .limit = 0x3F7,
        .info = PORT | STD | INUSE},

    {// Secondary control port
        .start = 0x376,
        .limit = 0x377,
        .info = PORT | STD | INUSE}
};

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
    dword size_of_entry;
    dword current_address;
    int i;
    for (int i = 0; i<cur_iorsc-1; i++)
    {
        size_of_entry = resources[i].limit - resources[cur_iorsc].start;

    }
    return 0;
}

/**
 * @brief Get the Int Info object, not for drivers
 * @param v IRQ number
 * @return A pointer to the interrupt, 4-bit normalized
 */
PInterrupt FastGetIntInfo(byte v)
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
 * @param bmp_intlines A bitmap of the interrupt lines
 * @return Notification
 * @retval RQINT_INUSE32  In use by another driver
 * @retval RQINT_STANDARD This interrupt is stanard hardware
 * @retval RQINT_TAKEN 0  Taken without any issues
*/
__DRVFUNC void IntrEnable(word bmp_intlines)
{
}

/*+===========================
Hardware detection functions
==============================*/


/// @brief Detect which interrupt are unused by ISA devices
/// @section Explaination
/// Windows 9x has a special IO.SYS that detects modifications
/// to interrupt vectors so that it can call 16-bit drivers.
/// A program called GRABIRQ.SYS records modifications to IRQ
/// vectors by hooking the DOS interrupts for that purpose.
static void DetectFreeInt(void)
{
}


static void DetectPs2Mouse(void)
{
}

/// @brief Detect how many COM ports there are and where they
/// are located by reading BDA. The IO base is usually standard
/// but some BIOSes allow ports to be changed.
/// If a computer has more than one serial port
/// @section Notes
/// then IRQ 3 and 4 are both for COM
/// COM2 and COM4 => IRQ#3
/// COM1 and COM3 => IRQ#4
 */

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


void InitResMGR(bool fpu_present, bool fpu_internal)
{
    DetectCOM();

    // Parallel ports do not share interrupts
    // If there are two, IRQ 5 is for the second one

    // Is there a PCI bus? If so, CF8-CFC are not available

    cur_iorsc = 20;
}


