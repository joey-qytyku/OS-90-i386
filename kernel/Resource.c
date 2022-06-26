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

#include <lib/Drivers.h>
#include <Resource.h>
#include <Type.h>
#include <Linker.h> // To get address of BDA

const psbyte KERNEL_OWNER = "KERNL386.EXE";

// Interrupt levels of each IRQ
static Interrupt interrupts[16] =
{ // TODO: ADD OWNERS
    [0] =   {STANDARD_32},  // Timer
    [1] =   {STANDARD_32},  // Keyboard
    [2] =   {STANDARD_32},  // Cascade
    [3] =   {UNKNOWN},      // COM, must detect
    [4] =   {UNKNOWN},      // Also COM
    [5] =   {UNKNOWN},      // LPT2 if present
    [6] =   {RECL_16},      // Floppy disk controller
    [7] =   {UNKNOWN},      // LPT1
    [8] =   {STANDARD_32},  // RTC
    [9] =   {RECL_16},      // Unassigned
    [10]=   {RECL_16},
    [11]=   {RECL_16},
    [12]=   {RECL_16},      // PS/2 mouse, 16-bit expected
    [13]=   {STANDARD_32},  // Math coprocessor
    [14]=   {STANDARD_32},  // ATA primary
    [15]=   {STANDARD_32}   // ATA secondary
    // If an interrupt comes from a 16-bit IRQ then it is assumed to be 16-bit
    // because no other driver has claimed it.
};

static dword cur_iorsc = 20;

/**
 * A flat list of a memory and IO space usage. Standard IO
 * spaces found on all PC-compatibles are pre-included.
 * 20 entries are reserved (0-19).
 */
static IO_Resource resources[MAX_IO_RSC] = {
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
        .start = 0x1F0},

    {// Primary ATA control port
        .start = 0x3F6,
        .limit = 0x3F7,
        .info = PORT | STD | INUSE},

    {// Secondary control port
        .start = 0x376,
        .limit = 0x377,
        .info = PORT | STD | INUSE}
};

void InitResMGR()
{
    const pword bda = (pword)phys(0x400);

    /* Check BIOS data area for number of serial ports
     * The beginning words are the COM port IO addresses
     * Zero indictates not present
    */
    byte i;
    for (i = 0; i < 4; i++) // Up to for COM ports on PC
    {
        if (bda[i] != 0) // then COM[i] exists
        {
            resources[cur_iorsc].start = bda[i];
            resources[cur_iorsc].limit = bda[i]+7;
            cur_iorsc++;
        }
    }

    /* If a computer has more than one serial port
     * then IRQ 3 and 4 are both for COM
     * 2 and 4 => IRQ3
     * 1 and 3 => IRQ4
    */

    // Parallel ports do not share interrupts
    // If there are two, IRQ 5 is for the second one
    // The first one is assumed to exist
}

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
__DRVFUNC Status Bus_AllocateIO(word size)
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

// If multiple ints must be read, call this once
// with vector zero and use it as an array
__DRVFUNC PInterrupt GetIntInfo(byte v)
{
    return &interrupts[v & 0xF];
}

__DRVFUNC Status RequestFixedLines(byte lines, PHandler handler, psbyte name)
{
    return 0;
}

