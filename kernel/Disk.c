
#include <Resource.h>
#include <Type.h>
#include <ATA.h>
#include <x86.h>

typedef struct { // TODO: What does this mean?
    word th;
    word ts;
    word tc;
    word cyls_per_head;

    word h, c, s;
}CHS_Params,*PCHS_Params;

typedef struct
{
    byte    exists:1;    /* If the status register is FF, no drive */
    byte    atapi:1;     /* If identify fails, it is ATAPI */
    byte    dsel:1;      /* Which disk is selected */
        /* This is because drive selection is slow
        // and requires a 15 ns delay (according to OSDev.org) */
    byte    flush_cache_cmd; /* 0x00 (NOP) if not supported */
}ATA_Drive;

ATA_Drive atapri, atasec;

dword CHS_to_LBA(PCHS_Params p)
{
    dword lba;
    // I *obviously* did not come up with this
    // https://pcrepairclass.tripod.com/cgi-bin/datarec1/chstolba.html
    lba = (p->c * p->th * p->ts) + (p->h * p->ts) + (p->s - 1);
    return lba;
}

// Returns the IO base or zero if error
static word SetParam(byte drive, word sectors, dword lba)
{
    byte bsectors, dsel;
    word io;

    // Drive numbers start at 2 for hard drives
    if (drive < 2 || sectors > 255)
        return 0;
    else if (sectors = 256)
        bsectors = 0;

    //Drive selecting requires reading the status 15 times

    dsel = drive_num & 1;
    if (drive_num)
        /*TODO*/;

    outb(io+2, wsectors);
    outb(io+3, (byte)(lba &  0xFF));
    outb(io+4, (byte)(lba >> 8);
    outb(io+5, (byte)(lba >> 16));

    return io;
}

/**
 * @drive: the drive number starting from 2
 * Return: -1 if no error but parameters invalid
 * error byte if error (never zero)
*/
sdword ATA_Read(const byte drive,
word sectors, const dword lba, const pvoid to)
{
    const word io = SetParam(drive, sectors, lba);
    byte stat;

    /**
     * If parameters invalid, fail with code -1
     * This means the error register was not read
     * but there is still an error
    */
    if (!io)
        return -1;

    outb(io+7, COM_READPIO);

    do {
        if (sectors == 0)
            break;

        insw(to, 256,io); // ?
        stat = inb(io+7);

        // Check for errors, return error register
        if (stat & STAT_ERR >0)
            return (dword)inb(io+1);

        sectors--;
    } while (STAT_BSY != 0 && stat & STAT_DRQ > 1);
}

void ATA_Write(const pvoid from, const byte drive,
word sectors, const dword lba)
{
    // Flush cache is only supported on ATA-4 from 1998
    // IDENTIFY
}

void EarlyInitATA()
{
    if (inb(ATAP+7) != 0xFF)
        atapri.exists = 1;
    if (inb(ATAS+7) != 0xFF)
        atasec.exists = 1;
}
