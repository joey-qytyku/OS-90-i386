# What is OS/90

OS/90 is the technology, or more specifically, the source code behind the release, which has not arrived yet and does not have a name. When the release is created from the source code, it is published under a different name, but with the same copyright and GPL license. Anyone can make a release under any name as long as they follow the rules in the GPL license included with the operating system code. GPLv2 is used to allow proprietary software to be included in the release, as most software of the DOS era was closed-source.

An analogy would be Windows NT, which is the technology made by Microsoft that they build on top of to release binary versions of the Windows operating system.

In all documentation, OS/90 is reffered to as an operating system.

Minimum requirements
|Spec|Minimum|Recommended|Premium|
-|-|-|-
RAM | 2MB      | 4MB       | 16MB
CPU | i386SX   | i486DX    | Pentium CPU
PC  | PC/AT    | PC/AT     | PS/2 compatible
OS  | DOS 3.0  | -         | -
Bus | ISA      | ISA w/PnP | PCI (any version)

* OS/90 is a Plug-and-play operating system. Enable this option in the BIOS. If the system does not have a PnP BIOS, it will run the same way.
* DOS should have at a decent amount of memory free to prevent out-of-memory issues

# Warnings

* Floppy disk support is experimental

This technology is infamously unreliable and I do not have computer with a floppy drive to test certain features.

* Do not use a compressed drive or 32-bit disk access will fail!

The kernel will call the 16-bit filesystem but traps disk access to perform it in protected mode for performance.

* Do not use disk cache software
I have not tested this, but SMARTDRV will cache disk contents to memory, sometimes XMS blocks. It will not be able to flush disk contents when the OS boots.

* Summary
Run DOS light for compatibility.

# Editions

There are editions for computers with different hardware. The -march is for the 386, so compatibility is garaunteed regardless of the CPU being used. Processor specific instructions may be used but only after being detected in real-time.

|Edition|Compiler Tuning|Included Drivers|
-|-|-
Type C| i386      | ~
Type B| i486      | ISAPNP
Type A| Pentium   | PCI, ISAPNP

# Build from Source

A Unix-like environment

The makefile needs to be modified to refference the appropriate toolchain.

NASM, DOSBox, git, qemu, and make must be installed. Once everything is set up, simply type ./Build.sh (chmod +x Build.sh if it does not work).

# Installation

The install files are provided in the release ZIP file. Unzip and copy to a CDROM or a series of floppy disks. Then XCOPY the files to a directory named OS90. Add OS90 to the path.

# Uninstall

Uninstalling is easy :)

OS/90 is self-contained and does not modify the DOS system at all. Delete the files in OS90. CONFIG.SYS must be updates to remove the GRABIRQ.SYS driver.

# Update

Reinstalling OS/90 is necessary to update. User files are not stored in the OS90 directory.

# Limitations

In the current design, the 32-bit userspace runtime has not been specified. It will be in the future. Filesystem access is 16-bit only, but disk access is 32-bit for performance reasons.

# After install

See boot.md for some information on the boot process and setting up the bootloader.

# Q&A

Q: Is OS/90 a real operating system or an interface for DOS?
A: I think it is an operating system. It has a multitasking kernel and driver interface, as well as 16-bit and 32-bit userspace. It is similar to Windows 3.1 enhanced mode and 95, both of which I consider to be operating systems. Despite this, DOS is a critical component and is used for more than just booting up.

Q: Is there plug and play support?
A: Yes. The OS is designed around plug-and-play functionality. There will never be plug and play COM (serial) device PnP. Just use Windows 95 for that. 16-bit plug-and-play drivers also work.

Q: Which PC busses are supported?

A: Theoretically all of them, but ISA PnP and PCI are sure to work best because they have specific drivers.

Q: Does this run on modern computers?

A: As long as it has a PC BIOS (even EFI-CSM) and 32-bit disk access is turned off (or IDE compatibility) for using SATA drives, there should be no problems. PCIe is compatible with PCI in software.

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

GUI? ACPI (yuck)?

# Credits

Matt Godbolt - godbolt.org is a very useful tool/
