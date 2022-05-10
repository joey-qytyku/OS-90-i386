#include <Type.h>

#define MAX_STD_RSC 64
#define MAX_IO_RSC 64

#define MEM 0
#define AVAIL 0

#define PORT 1
#define STD 2
#define INUSE 4

enum InterruptLVL
{
    UNKNOWN = 0,
    OPEN32 = 1 << 4,
    RECL_16 = 2 << 4,
    TAKEN_32 = 3 << 4,
    STANDARD_32 = 4 << 4,
    RECL_32 = 5 << 4
};

typedef struct
{
    dword start;
    dword limit;
    dword info;
} IO_Resource; // IO ports or memory mapped IO

int RequestIRQ(byte irq, void (*handler)(void*));
