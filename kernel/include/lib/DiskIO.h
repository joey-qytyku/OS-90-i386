/*
 * KDM API functions. The following are disk IO routines for reading and
 * writing to logical and physical volumes
 */

#include <Type.h>

#define __sdcall __attribute__((stcall))
enum {
	FDISK,
	ATAPI,
	FLOPPY // Drive 0 and 1 are always floppy
};

// Packed struct used for compatibility with different compilers
typedef struct __attribute__((packed))
{
	byte valid; // If not valid, this drive with
	word block_size;
	word disk_type;

}DiskInfStruct;

/*
 * Get information on physical disks, not logical volumes
 * Physical disks are ordered from 0-255 as with virtual volumes.
*/
__sdcall extern void KDM_PhysicalDiskInf(byte disk_num, DiskInfStruct *inf);
__sdcall extern void KDM_ReadBlocks(byte disk_num, word count, void *copyto);
