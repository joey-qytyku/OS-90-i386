
#include <Resource.h>
#include <Type.h>
#include <ATA.h>
#include <x86.h>

typedef struct {
    word th;
    word ts;
    word tc;
    word cyls_per_head;

    word h, c, s;
}CHS_Params;

typedef struct __attribute__((packed))
{
    byte    exists:1;    // If the status register is FF, no drive
    byte    atapi:1;     // If identify fails, it is ATAPI
    byte    dsel:1;      // Which disk is selected
        // This is because drive selection is slow
        // and requires a 15 ns delay (according to OSDev.org)
}ATA_Drive;

ATA_Drive atapri, atasec;

dword CHS_to_LBA(CHS_Params *p)
{
    dword lba;
    // I *obviously* did not come up with this
    // https://pcrepairclass.tripod.com/cgi-bin/datarec1/chstolba.html
    lba = (p->c * p->th * p->ts) + (p->h * p->ts) + (p->s - 1);
    return lba;
}

static int SetParam(byte drive_num, word sectors, dword lba)
{
    byte bsectors;
    if (sectors == 256)
        bsectors = 0;
    if (sectors > 255)
        return 1;
    word io;
    switch (drive_num) {
        case 0:
            //
        break;
    }

    outb(io+2, wsectors);
    outb(io+3, (byte)(lba &  0xFF));
    outb(io+4, (byte)(lba >> 8);
    outb(io+5, (byte)(lba >> 16));
}

void ATA_Read(word io, word sectors, dword lba, pvoid to)
{
    byte stat;

    SetParam(io, sectors, lba);
    outb(io+7, COM_READPIO);

    do {
        if (sectors == 0)
            break;
        insw(to, 256,io+7);
        stat = inb(io+7);
        sectors--;
    }
    while (STAT_BSY != 0 && stat & STAT_DRQ > 1);
}

void ATA_Write()
{
    // Flush cache
}

void EarlyInitATA()
{
    if (inb(ATAP+7) != 0xFF)
        atapri.exists = 1;
    if (inb(ATAS+7) != 0xFF)
        atasec.exists = 1;
}
