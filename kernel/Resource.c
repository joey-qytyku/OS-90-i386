/* Resource management for the IBM PC/AT architecture and above
 *
 *
 */

#include <Type.h>
#define MAX_STD_RSC 64
#define MAX_IO_RSC 64

#define MEM 0
#define AVAIL 0

#define PORT 1
#define STD 2
#define INUSE 4

enum InterruptLVL
{
    UNKNOWN = 0,
    OPEN32 = 1 << 4,
    RECL_16 = 2 << 4,
    TAKEN_32 = 3 << 4,
    STANDARD_32 = 4 << 4,
    RECL_32 = 5 << 4
};

typedef struct
{
    dword start;
    dword limit;
    dword info;
} IO_Resource; // IO ports or memory mapped IO

static IO_Resource iorsc[64];
// Partial arrays are placed in data section
// Empty arrays go in BSS and are not in the final binary

// Interrupt levels of each IRQ
static byte interrupts[16] =
{
    [0] =   STANDARD_32,    // Timer
    [1] =   STANDARD_32,    // Keyboard
    [2] =   STANDARD_32,    // Cascade
    [3] =   UNKNOWN,        // COM, must detect
    [4] =   UNKNOWN,        // Also COM
    [5] =   UNKNOWN,        // LPT2 if present
    [6] =   STANDARD_32,    // Floppy disk controller
    [7] =   UNKNOWN,        // LPT1
    [8] =   STANDARD_32,    // RTC
    [9] =   RECL_16,        // Unassigned
    [10]=   RECL_16,
    [11]=   RECL_16,
    [12]=   RECL_16,        // PS/2 mouse, 16-bit expected
    [13]=   STANDARD_32,    // Math coprocessor
    [14]=   STANDARD_32,	// ATA primary
    [15]=   STANDARD_32		// ATA secondary
    // If an interrupt comes from a 16-bit IRQ then it is assumed to be 16-bit
    // because no other driver has claimed it.
};

IO_Resource Standard[] = {
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

    {.start = 0x376,
     .limit = 0x377,
     .info = PORT | STD | INUSE}
};

void SetupStd_IO(IO_Resource (*array)[MAX_IO_RSC])
{
    volatile word *bda = 0x400;
    // Check BIOS data area for number of serial ports

    /* If a computer has more than one serial port
     * then IRQ 3 and 4 are both for COM
     * 2 and 4 => IRQ3
     * 1 and 3 => IRQ4
     */

    // Parallel ports do not share interrupts
    // If there are two, IRQ 5 is for the second one
    // The first one is assumed to exist
}
