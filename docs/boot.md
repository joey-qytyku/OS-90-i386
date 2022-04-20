# kuDOS Bootloader

kuDOS is loaded by BOOT386.SYS. This is a fake device driver that runs before anything else on the system. It MUST be the last DOS driver loaded if there are other drivers that should be loaded. The kernel is a flat binary file loaded into extended memory starting at 0x100000. When the kernel finishes its own initialization process, it will run autoexec.bat. TSRs are loaded this way.

The bootloader changes directory to KUDOS\ and tries to open KERNL386.SYS. If it fails to find this then DOS will simply boot normally. It seeks to the end of the file to get the total size. Blocks are read into the 1M area using unreal mode. String instructions with address size override profixes are used to copy the data (this is possible).

128 kilobytes of conventional memory are allocated and used for ISA DMA.

# Configuration

Config.sys should probably not use a boot menu because one is a already included. EMM386 OR ANY OTHER EMS DRIVER/EMULATOR MUST BE TURNED OFF OR THE WHOLE COMPUTER WILL CRASH. HIMEM.SYS does no harm but the XMS API is emulated by the kernel.

Add this entry to the last line of config.
`DEVICE=C:\KUDOS\BOOT386.SYS`

.SYS drivers for do not work at all and are simply ignored. This is because DOS drivers can be either blockdevs or chardevs, all of which are 32-bit on OS/90. There are fake device drivers like EMM386 (already mentioned, dont use it), HIMEM.SYS, and maybe some device drivers for other devices. These also won't work. The config line should be added last so that other drivers load and that MS-DOS configures in the desired way. This is a restrictive configuration, I know, but I think it works best. Autoexec.bat runs in a controlled environment and exclusive access to the HMA is assured.

As soon as BOOT386.SYS is loaded by DOS, the OS will boot immediately. There is no boot menu, but one can be implemented by tweaking Config.sys. That way, one can switch to DOS if OS/90 turns out to not be that good.

# Kernel Startup

The kernel will use fully 16-bit drivers unless it can find replacements, with the exceptions being the keyboard, timer, and VGA text screen, which are implemented in the main kernel image. The exception is floppy and hard disk access. This is always 32-bit and there are pre-included drivers.

