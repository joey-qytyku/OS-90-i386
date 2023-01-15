#ifndef DEBUG_H
#define DEBUG_H

typedef VOID (*OUTPUT_DRIVER)(BYTE);
VOID APICALL Hex32ToString(DWORD, PBYTE);
VOID APICALL Uint32ToString(DWORD, PBYTE);
VOID APICALL KeLogf(OUTPUT_DRIVER, IMUSTR restrict, ...);
VOID FatalError(DWORD);

#endif