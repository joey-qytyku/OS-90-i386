# Supported Programs

* 16-bit DOS Programs
* DPMI programs (e.g. DOOM, DJGPP, various games)
* OS/90 programs

DPMI is designed to run on top of DOS and cannot be used to implement the entire system. While multitasking is possible under DPMI, it is hard to use loadable libraries as there is no standard executable format. DPMI is best suited for standalone applications that do everything in a single binary.

OS/90 programs are capable of loading dynamic libraries.

# Executable Format

Executables are 32-bit PE/COFF files. They may specify the console subsystem in the header as there is no GUI right now.

# DPMI Implementation Details

## LDT

DPMI applications share the same local descriptor table.

## Virtual Interrupts
