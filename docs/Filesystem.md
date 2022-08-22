# OS/90 Filesystem

By default, all filesystem operations are performed by MS-DOS. A filesystem driver can take over the INT 21H filesystem calls and allow for 32-bit filesystem access.

Disk access is also 16-bit and uses the BIOS. As with FS, a driver can replace the BIOS and make it 32-bit.

# Device Files

Floppy drives are registered as FDA and FDB. IDE/ATA drives are DSKA-DSKD. Disk drives are in the BLOCK directory.

Sectors are garaunteed to be 512 bytes in both cases.
