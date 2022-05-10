#ifndef ATA_H
#define ATA_H

#define ATAP 0x1F0
#define ATAS 0x170

#define CTLP (ATAP + 0x200)
#define CTLS (ATAS + 0x200)

// And plus != to zero to check if set
// Error register
enum ATA_ERROR {
    AMNF  = 1,
    TKZNF = 2,
    ABRT  = 4,
    MCR   = 8,
    IDNF  = 16,
    MC    = 32,
    UNC   = 64,
    BBK   = 128,
};

enum ATA_COMMANDS {
    COM_READPIO,
    COM_WRITEPIO
};

#endif
