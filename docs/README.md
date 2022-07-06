# OS/90

OS/90 is a work-in-progress operating system inspired by Windows 9x and 3.x architecturally. It is a 32-bit operating system that uses a CLI.

Minimum requirements
|Spec|Minimum|Recommended|Premium|
-|-|-|-
RAM | 2MB      | 4MB       | 16MB
CPU | i386SX   | i486DX    | Pentium I or II
PC  | PC/AT    | PC/AT     | PS/2 compatible
OS  | DOS 3.0  | -         | -
Bus | ISA      | ISA w/PnP | PCI (any version)


# Warnings

* Do not use a compressed drive or 32-bit disk access will fail!

The kernel will call the 16-bit filesystem but traps disk access and perform it in protected mode for performance.

* Do not use .SYS drivers block device access, it will not work!
The entire OS has no support at all for the DOS driver model (.SYS) and never will. Character device drivers may still work if they do not perform direct hardware access.'

* Do not use disk cache software
I have not tested this, but SMARTDRV will cache disk contents to memory, sometimes XMS blocks. It will not be able to flush disk contnets when the OS boots.

# Things That do NOT work

Programs for DOS run in a protected environment, so direct hardware access will probably fail (IO ports). Changing video modes is possible.

# Editions

There are editions for computers of different age.

|Edition|Compiler Tuning|Included Drivers|
-|-|-
Type C| i386      | ~
Type B| i486      | ISAPNP
Type A| Pentium   | PCI, ISAPNP, UHCI

Because instructions execute completely differently, each edition is tuned for a certain processor.

# Installation

The install files are provided in the release ZIP file. Unzip and copy to a CDROM or a series of floppy disks. Then XCOPY the files to a directory named OS90. Add OS90 to the path.

# Uninstall

Uninstalling is easy :)

OS/90 is self-contained and does not modify the DOS system at all. Delete the files in OS90.

# Update

Reinstalling OS/90 is necessary to update. User files are not store in the OS90 directory.

# Limitations

In the current design, the 32-bit userspace runtime has not been specified. It will be in the future. Filesystem access is 16-bit only, but disk access is 32-bit for performance reasons.

# After install

See boot.md for some information on the boot process and setting up the bootloader.

# Goals

The goal of OS/90 is to create an operating system for very old computers that maximizes DOS compatibility. The DOS compatibility, which extends to some drivers, will hopefully make this OS more useful, as I doubt many developers are going to make 32-bit native drivers for this specific OS. I also thought it would be more fun to work on a DOS/Windows-like operating system. Unix seems done to death in the OSDev community, probably because most of us use Unix-like tools.

# Q&A

Q: Is OS/90 a real operating system or an interface for DOS?
A: I think it is an operating system. It has a multitasking kernel and driver interface, as well as 16-bit and 32-bit userspace. It is similar to Windows 3.1 enhanced mode and 95, both of which I consider to be operating systems. Despite this, DOS is a critical component and is used for more than just booting up.

Q: Is there plug and play support?
A: Yes. The OS is designed around plug-and-play functionality. There will never be plug and play COM (serial) device PnP. Just use Windows 95 for that. 16-bit plug-and-play drivers also work.

Q: Which PC busses are supported?

A: Theoretically all of them, but ISA PnP and PCI are sure to work best because they have specific drivers.

Q: Does this run on modern computers?

A: As long as it has a PC BIOS (even EFI-CSM) and 32-bit disk access is turned off (or IDE compatibility) for using SATA drives, there should be no problems. PCIe is compatible with PCI. Both PCI configuration mechanisms are supported.

Q: Do DOS drivers work?

A: They should, but are certainly less stable. DOS drivers will probably be the only option for the majority of expansion cards.

Q: What type of kernel design is used?

A: A hybrid design is used. There are drivers in KERNL386.EXE but they are only for standard PC hardware. Almost everything else goes in a driver outside of the kernel but still in the same address space.

Q: I want to write software for OS/90, what resources are available?

A: Everything in DOCS\ is relevant for kernel-mode development. Functions available to drivers are prefixed with \__DRVFUNC and are added to the kernel symbol table with EXPORT_SYM.

Source code is the most reliable documentation. If there are any questions, feel free to ask me.

Q: Which version of DOS is best?
A: In theory, FreeDOS is the best because it has LFN support, but it has many extra packages that could cause compatibility issues. Do not use EMM386 or JEMMEX. Disabling disk caching software may also be a good idea.

## Pipe Dreams

I may work on a GUI at some time in the distant future. ACPI 1.0 and APM support?

# Credits

Just me rn.
