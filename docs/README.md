# OS/90

The minimum requirements are as follows:
* 2 MiB of RAM
* i386SX
* PC/AT compatible
* 20 MiB hard drive (note binary measurments)
* MS-DOS 3.0 or higher

The recommended hardware:
* 4 MiB of RAM
* i386DX
* PC/AT or PS2
* 30 MB

The maximum amount of ram is 1 GB.

Do not use EMS hardware. No LIM EMS cards are plug and play so they will only cause problems once PnP support is added. LIM EMS is emulated by the kernel and allows for many more memory blocks.

Installing on MS-DOS 7.0 may be possible, but this version is usually bundled with Windows. Filesystem access may be broken as DOS 7 can access large disks

# Installation

The install files are provided in the release ZIP file. Unzip and copy to a CDROM or a series of floppy disks. Then XCOPY the files to a directory named OS90.

# Uninstall

Uninstalling is easy :)

OS/90 is self-contained and does not modify the DOS system at all. Delete the files in OS90. Change Config.sys, restore a backup, or otherwise remove the bootloader from the file. DOS should boot normally.
