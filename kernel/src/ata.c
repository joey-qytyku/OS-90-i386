#include <owintrin.h>


#define C_IDENTIFY_MASTER
#define C_IDENTIFY_SLAVE

#define ATA0_IO 0x1F0
#define ATA0_CPORT 0x3F0

#define ATA1_IO 0x170
#define ATA1_CPORT 0x376

typedef struct {
	char in_pio_mode;
	unsigned char sector_size;
	unsigned char pci_primary_device;
	unsigned long pci_bar[5]
}ata_physdev_info;

enum ata_errors {
	AMNF =  1,
	TKZNF=  1<<1,
	ABRT =  1<<2,
	MCR  =  1<<3,
	IDNF =  1<<4,
	MC   =  1<<5,
	UNC  =  1<<6,
	BBK  =  1<<7
};

#define ATA_CHECK_ERR_TYPE(CODE, CHECKVAL)\
((CHECKVAL &TYPE)!=0)
// Correct? Yes

void init_ata(unsigned short master, unsigned short slave)
{

}
