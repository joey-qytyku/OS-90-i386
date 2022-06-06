# Kernel API

KAPI is the application programming interface provided by the kernel to ring 0 programs called __drivers__. All functions use __cdecl__.

## Int86(pvoid mem, page mem4k, GeneralRegdump rd)

Call a 16-bit interrupt. A pointer to the memory is required. If mem == NULL the real mode memory is used and page mappings are not changed, as this is the default for the kernel.

## int PnP(word func, word node ...)

Call protected mode PnP call. This function copies values to a 16-bit stack segment and calls PnP BIOS functions

## void DriverInitFail()

The driver is incapable of continuing the initialization process and must exit to the kernel.

# Bus Drivers

The bdev is supposed to enumerate a bus and report to other drivers the devices that are connected. The kernel assists with this process.

The main idea as that the bdev arbitrates system resources with the supervision of the kernel and dispatches hardware events.

The bus should never be enumerated by drivers. Resources should be reassigned by the bus driver if possible.

bdevs can use other bdevs, such as USB using PCI.

## Bus_TakeLine(dword handle, byte lines)

When  line is taken, dispatching must be handled by the driver so that a proper ISR is called.

When an interrupt is sent here, the kernel will call the bus driver.

The kernel only has one ISR for each interrupt an c

## Busdev_RequestAddHandler(dword handle)

## Busdev_RequestSolitaryIRQ(dword handle);

Some drivers may want a solitary IRQ (no share). Sound cards, for example, frequently broadcast interrupts and sharing becomes inefficient.

Device drivers must support all three cases.

returns:
DEV_YES
DEV_NO
DEV_NOT_SUPPORTED

# Advice

Drivers should make sure that the device is able to run under any condition because they cannot truly terminate and will end up wasting memory. Drivers can give up initialization but this should be used sparingly. For example, a sound drv should not give up because it did not get a solitary IRQ.

# Examples
## PCI.DRV

This driver is preincluded.


