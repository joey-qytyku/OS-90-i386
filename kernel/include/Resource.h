#include <Type.h>

#define MAX_STD_RSC 64
#define MAX_IO_RSC 64

#define MEM 0
#define AVAIL 0

#define PORT 1
#define STD 2
#define INUSE 4

#define HANDLERS 4

typedef int (*PHandler)(void*);

enum InterruptLVL
{
    UNKNOWN = 0,
    OPEN32,
    RECL_16,
    TAKEN_32,
    STANDARD_32,
    RECL_32
};

typedef struct {
    enum InterruptLVL intlevel;
    PHandler handlers[HANDLERS];
    int index;
    char owners[HANDLERS][8]; // FAT case
}Interrupt,*PInterrupt;

typedef struct {
}COM_Port;

typedef struct
{   dword start;
    dword limit;
    dword info;
}IO_Resource; // IO ports or memory mapped IO

int RequestIRQ(byte irq, void (*handler)(void*));
