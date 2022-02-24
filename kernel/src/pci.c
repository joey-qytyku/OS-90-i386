#include <ata.h>
#define PCICFG_DATA 0xCFC
#define PCICFG_ADDR 0xCF8

// reg must be dword aligned
unsigned short pcicfg_read32(
byte bus, byte dev, byte func, byte reg)
{
	unsigned long addr = 0x80000000;

	addr =
		((func&7)  << 8) |
		((dev&)    << 11)|
		((reg&0xFC)<< 2);

	outpl(0xCF8, address);
	return inpl(0xCFC);
}

/*
WT caching for MMIO regions
How doe sthis interract with the memory manager.
Am I using paging?
*/

void enumerate_pci()
{
	int bus, dev, reg;
	unsigned long en = 0x80000000;

	for (bus = 0; bus <= ){
		for {dev = 0; dev == 0;(dev+1)&}
		{}
	}
}
