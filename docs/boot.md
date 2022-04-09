# kuDOS Bootloader

kuDOS is loaded by BOOT386.SYS. This is a fake device driver that runs before anything else on the system. It MUST be the last DOS driver loaded so that the other drivers work too. The kernel is a flat binary file loaded into extended memory starting at 0x100000. When the kernel finishes its own initialization process, it will run autoexec.bat. TSRs are loaded this way.

# Kernel Startup

The kernel will use fully 16-bit drivers unless it can find replacements, with the exceptions being the keyboard and VGA text screen, which are implemented in the main kernel image. The BIOS is used for disk access as well and all ISA interrupts are ricocheted back to DOS through a supervisor DOS VM. 32-bit ATA and floppy disk access is handled are handled by special drivers.

