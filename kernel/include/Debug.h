#ifndef DEBUG_H
#define DEBUG_H

#include <Type.h>

#define KDGB_OD KernelPutchar
// What was this for? I made the function thread safe.
#define ERROR_IF_ISR()

typedef VOID (*OUTPUT_DRIVER)(BYTE);

extern VOID APICALL Hex32ToString(DWORD, PBYTE);
extern VOID APICALL Uint32ToString(DWORD, PBYTE);
extern VOID APICALL KeLogf(OUTPUT_DRIVER, IMUSTR restrict, ...);
extern VOID FatalError(DWORD);

extern VOID _KernelPutchar(BYTE ch);

#endif
