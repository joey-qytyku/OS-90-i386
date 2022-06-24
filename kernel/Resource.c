/* Resource management for the IBM PC/AT architecture and above
 * it uses a flat list for all resource types
 *
 */

#include <Resource.h>
#include <Type.h>
#include <Linker.h>     // To get address of BDA

// Get the address of this, not the name
char KERNEL_OWNER[8] = {'K','E','R','N','L','3','8','6'};

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
// First 20 are reserved

IO_Resource resources[MAX_IO_RSC] = {
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

static dword cur_iorsc = 20;

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
    C_memcpy(&resources[cur_iorsc], new_res, sizeof(IO_Resource));
    cur_iorsc++;
}

__DRVFUNC Status Bus_AllocatePortIO(word size)
{
}

// If multiple ints must be read, call this once
// with vector zero and use it as an array
__DRVFUNC PInterrupt GetIntInfo(byte v)
{
    return &interrupts[v & 0xF];
}

// Claiming interrupts can be done repeadedly to change the ISR
__DRVFUNC Status RequestFixedLines(byte lines, PHandler handler, pchar name
){
    return 0;
}
 
KERNEL_XPSYM(RequestFixedLines);
KERNEL_XPSYM(GetIntInfo);
KERNEL_XPSYM(AddIOMemRsc);
