#ifndef DRIVERS_H
#define DRIVERS_H

#include <Type.h>

// This is used only by the kernel
#define KERNEL_XPSYM(name)
#define __DRVFUNC

enum {
    DRVINF_BUS, // This is a bus driver, so bus commands are valid
    DRVINF_INT, // Allows driver to call interrupt functions
    DRVINF_DMA  // The driver takes DMA chanels (ISAPNP)
};

typedef struct {
    /* Program info */
    dword flags;
    char *name;
}DriverParamBlock;

#endif /* DRIVERS_H */
