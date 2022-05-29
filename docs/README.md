# OS/90

OS/90 is a work-in-progress operating system inspired by Windows 9x and 3.x architecturally. It is a 32-bit operating system that uses a CLI.

The minimum requirements are as follows:
* 2 MiB of RAM
* i386SX
* PC/AT or compatible
* MS-DOS 3.0 or higher
* ISA bus

This setup would not be that slow, but it is the minimum.

The recommended hardware:
* 8 MiB of RAM
* i386DX or i486 DX2
* PC/AT
* ISA bus with PnP support

Premium Setup:
* PS/2 compatible
* 16 MiB of RAM
* Pentium
* PCI bus

PCI and ISA are officially supported buses. VLB, MCA, and EISA should work too because OS/90 keeps the BIOS default settings.

A floating point unit is not required for i386 users. The OS code never uses the floating point unit, but does detect its presence and saves registers on pre-emption if a process uses them and it is in fact present. There are no build options for processors; both i386 and i486+ are supported, but the i486 GCC compiler and tuning is used.

Disk space is TBD.

The maximum amount of ram is 1 GB. There is no reason for anything higher. A system should boot if it has more.

Do not use EMS hardware. No LIM EMS cards are plug and play so they will only cause problems once PnP support is added. LIM EMS is emulated by the kernel and allows for many more memory blocks.

Plug and play cards should not co-exist with non-pnp cards. If this is required, the legacy ISA card drivers should be loaded first so that the resource manager can reserve its IRQs, DMAs, and ports. PCI may provide the better experience as it is very well documented.

Installing on MS-DOS 7.0 may be possible, but this version is bundled with Windows, so there is no point.

# Installation

The install files are provided in the release ZIP file. Unzip and copy to a CDROM or a series of floppy disks. Then XCOPY the files to a directory named OS90. Add OS90 to the path.

# Uninstall

Uninstalling is easy :)

OS/90 is self-contained and does not modify the DOS system at all. Delete the files in OS90.

# Update

Reinstalling OS/90 is necessary to update. User files are not store in the OS90 directory.

# Limitations

In the current design, the 32-bit userspace runtime has not been specified. It will be in the future. Filesystem access is 16-bit only, but disk access is 32-bit for performance reasons.

OS/90 may not work properly with MS-DOS 7 and above. Windows ME reports as MS-DOS 8 and is completely gutted in terms of DOS functionality so that is not a concern.

# After install

See boot.md for some information on the boot process and setting up the bootloader.

# Goals

The goal of OS/90 is to create an operating system for very old computers that maximizes DOS compatibility. The DOS compatibility, which extends to some drivers, will hopefully make this OS more useful, as I doubt many developers are going to make 32-bit native drivers for this specific OS. I also thought it would be more fun to work on a DOS/Windows-like operating system. Unix seems done to death in the OSDev community, probably because most of us use Unix-like tools.

# FAQ

Q: Is OS/90 a real operating system or an interface for DOS?
A: I think it is an operating system. It has a multitasking kernel and driver interface, as well as 16-bit and 32-bit userspace. It is similar to Windows 3.1 enhanced mode and 95, both of which I consider to be operating systems. Despite this, DOS is a critical component and is used for more than just booting up.

Q: Is there plug and play support?
A: Yes. The OS is designed around plug-and-play functionality. There will never be plug and play COM (serial) device PnP. Just use Windows 95 for that. 16-bit plug-and-play drivers also work.

Q: Which PC busses are supported?
A: Theoretically all of them, but ISA and PCI are sure to work best.

Q: Does this run on modern computers?
A: As long as it has a PC BIOS (even EFI-CSM) and 32-bit disk access is turned off (or IDE compatibility) for using SATA drives, there should be no problems. PCIe is compatible with PCI. Both PCI configuration mechanisms are supported.

Q: Do DOS drivers work?
A: They should, but are certainly less stable. DOS drivers will probably be the only option for the majority of cards.

## Pipe Dreams

I may work on a GUI at some time in the distant future. ACPI 1.0 and APM support?

# Credits
