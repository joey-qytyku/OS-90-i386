#include <DriverLib/Drivers.h>
#include <Platform/Resource.h>
#include <Type.h>
#include "pci.h"

const char *driver_name = "PCI.DRV";

DriverParamBlock pci = {
    .name = driver_name
};

static IO_Resource mech2_conf_ports =
{
    .owner = driver_name, .start = 0xC000,
    .limit = 0xCFFF, .info  = PORT | INUSE
};

static IO_Resource both_mechs_conf_ports =
{
    .start = 0xCF8, .limit = 0xCFF,
    .owner = driver_name, .info  = PORT | INUSE
}

typedef struct {
    byte bus, device, function, offset;
}CfgN1Addr;

static dword mechanism; // Faster than memory access for fnptr

/// @brief Find out of there is a PCI bus and the CFG mechanism
/// @section Details
/// The BIOS PCI
/// @retval 0 No PCI support
/// @retval 1 Mechanism 1
/// @retval 2 Mechanism 2
///
Status DetectCFG(void)
{
    // Address FFE6Eh is supposed to be the standard
    // entry point for running PCI BIOS calls
    // but I will not rely on this
}

Status PCI_Init(void)
{
    // 0xCF8-0xCFF are unavailable for both mechanisms
    AddIOMemRsc(both_mechs_conf_ports);

    if (DetectCFG == DETECT_CFG2) {
        AddIOMemRsc(mech2_port_space);
    }
}
