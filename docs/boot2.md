# Boot process and Legacy Drivers

## Goals and Summary of Design

OS/90 is designed to maximize DOS compatibility. It can load and execute DOS drivers and applications. 90.COM is the program that initializes the system. Before this runs, MS-DOS is loaded by the boot sector and parses CONFIG.SYS (see note). COMMAND.COM executes next. AUTOEXEC.BAT is executed by the COMMAND.COM and TSR drivers are loaded in conventional memory. 90.COM performs the boot process.

16-bit DOS drivers can be used by the operating system. Device drivers (not speaking of .SYS files in CONFIG) always hook interrupts (often an IRQ). DOS drivers cannot be loaded after this.

## 90.COM

This file is the main boostrap program. It requires HIMEM.SYS or some other XMS driver to be loaded. Only the 2.0 feature set is used, so older versions should work. XMS is used so that pre-existing drivers using extended memory blocks can work. The high memory area cannot be shared. Do not use DOS=HIGH or the system will overwrite the DOS kernel and crash the system.

Write to EMB directly? Bad idea?

# HMA Issues

The HMA is used for startup and later on for ISA DMA. XMS 2.0 lets only one program can use it at a time, and in this case that is the 32-bit kernel. Remove DOS=HIGH from confg.sys and anything that uses it.

# DOS drivers

16-bit drivers will work. The DOS .SYS driver model is unsupported by the 32-bit kernel, but the DOS kernel and programs can still use them within a DOS VM. TSR programs like MS Mouse will also function as expected as the mouse interrupt is expected to be 16-bit.

## ISA PnP drivers

There are 16-bit plug-and-play drivers for ISA cards. Such drivers will send the initiation key and perform configuration on a detected card when it recognizes the card identification. ISA PnP drivers presumably use the real-mode PnP BIOS interface to get CSN information.

OS/90 supports plug-and-play ISA and detects what resources have been assigned to each card. It does not reassign card select numbers as configured by the BIOS (This may be an issue).

In short, ISA plug-and-play cards can operate with 16-bit DOS drivers just fine.

# Notes

Upper memory blocks are not supported. EMM386 should __NEVER__ be used! Loading device drivers into a UMB will not do anything (they will be loaded low)

# Parameters

The PSP of the bootloader is passed to the kernel for parsing. They are case insensitive.

/NODRIVERS
