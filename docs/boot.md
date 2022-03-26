# Boot Process

The pogram LOADER.COM is responsible for bootstraping kuDOS. It currently requires XMS to be disabled before booting. If a 32-bit processor is not found, the program exits and prints an error message.

Requirements before running:
* 32-bit Intel processor or compatible
* IBM AT compatible, or better
* XMS and EMS driver is disabled
* MS-DOS 4.x or better

The kernel is loaded page-by-page into 0x110000, above the HMA. The bootloader then switches loads a temporary GDT and switches to 32-bit protected mode with the flat model. Before the kernel can be entered, it must first be mapped into the address space. Four page tables and a single page directory are set up that map the kernel to address C0000000h. Paging is enabled

* Load kernel to 110000h
* Load GDT, base of segments is 0
* Set CR3 to the page directory base
* Switch to protected mode and enable paging
  * At once
* Far jump thorugh code segment to C0000000h

Register state upon kernel entry is undefined. A stack and GDT must be set up by the kernel when it needs it.

# Kernel

When the kernel boots, it loads a new GDT, sets up a startup stack. This stack will only be used for initialization. When the OS enables multitasking and starts running programs, seperate kernel stacks will be used for each process.


