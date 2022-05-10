
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


dword CHS_to_LBA(CHS_Params *p)
{
    dword lba;
    // I *obviously* did not come up with this
    // https://pcrepairclass.tripod.com/cgi-bin/datarec1/chstolba.html
    lba = (p->c * p->th * p->ts) + (p->h * p->ts) + (p->s - 1);
    return lba;
}

typedef struct __attribute__((packed))
{
    byte    exists:1; // If the status register is FF, no drive
    byte    atapi:1;  // If identify fails, set
}ATA_Drive;

ATA_Drive atapri, atasec;

/* ATA PIO driver */

static void ATA_Read(word io, word sectors, dword lba, void *buff)
{
    // Drive select, I don't think doing it each time will be that slow

}

void EarlyInitATA()
{
    if (inb(ATAP+7) != 0xFF)
        atapri.exists = 1;
    if (inb(ATAS+7) != 0xFF)
        atasec.exists = 1;
    // IDENTIFY
    outb(inb())
}
