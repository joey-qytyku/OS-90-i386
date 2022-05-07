# Boot process and Legacy Drivers

## Goals and Summary of Design

OS/90 is designed to maximize DOS compatibility. It can load and execute DOS drivers and applications. 90.COM is the program that initializes the system. Before this runs, MS-DOS is loaded by the boot sector and parses CONFIG.SYS (see note). COMMAND.COM executes next. AUTOEXEC.BAT is executed by the COMMAND.COM and TSR drivers are loaded in conventional memory. 90.COM performs the boot process.

16-bit DOS drivers can be used by the operating system. Device drivers (not speaking of .SYS files in CONFIG) always hook interrupts (often an IRQ). DOS drivers cannot be loaded after this.

## 90.COM

This file is the main boostrap program. It requires HIMEM.SYS or some other XMS driver to be loaded. Only the 2.0 feature set is used, so older versions should work. XMS is used so that pre-existing drivers using extended memory blocks can work.

The algorithm:
* Check if XMS is present
  * If not, error
* If present:
  * Request entire HMA
    * If not available, error
    * If available
      * Extend DS and ES segments using unreal mode
      * Generate page tables and directory (total 12288 bytes)
      * Open kernel file
      * Read into buffer
      * Copy 4096 to HMA+12288+current_page
      * Load GDT
      * Switch to protected mode with paging on
      * Set segment registers
      * Jump to the kernel at 0xC0000000

# Notes

Upper memory blocks are not supported. EMM386 should __NEVER__ be used! Loading device drivers into a UMB will not do anything (they will be loaded low)
