Bus drivers are responsible for arbitrating system resources. Other device drivers communicate with the bus driver to request control of such resources. The KAPI provides a bus api for this purpose.

Drivers for PCI devices interract with PCI.DRV.

Bus drivers WIP:
* PCI
* COM
* PnP ISA
* USB UHCI

# Kernel Resource Management

## Interrupts

### Standard config

ATA primary and secondary are both unavailable for other uses, even if the secondary channel has no disks.

|IRQ|Purpose|
-|-|
0|8253 PIT
1|8042 Keyboard
2|Cascase
3|COM 2 and 4
4|COM 1 and 3
5|LPT 3 or sound card
6|Floppy disk controller
7|LPT 1 and 2

There can only be up to 3 LPT ports. COM 2 and 4 may not always be present.

## Overview

To support 16-bit drivers, interrupts can be 16-bit or 32-bit. 16-bit interrupts can be reclaimed later. The bdrv is required to preserve 16-bit interrupts and not allocate them to bus devices.

Interrupts can be 32-bit non-reclaimable or 16-bit reclaimable. 16-bit drivers used for DOS can be replaced and the drivers for such devices must make sure that the device is properly initialized. For example, assume that the mouse is already configured and not off after startup.

int Bus_ProbleLines();

returns a bitmap of 16-bit interrupts (bits on).

int Bus_ClaimLines(word map, char *bus_name)

returns:
BUS_INT_SUCCESS_FREE
BUS_INT_SUCCESS_16
BUS_INT_UNRECL
BUS_INT_STANDARD (error)


## ISA DMA

ISA DMA sends interrupts on the behalf of the requesting device. DMA also cannot be shared. A bus driver can own DMA channels but should not for busses with their own DMA systems.

# Bus Drivers

bdrvs enumerate the bus and report existing devices to other drivers.
