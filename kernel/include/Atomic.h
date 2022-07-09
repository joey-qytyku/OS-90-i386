#ifndef MUTEX_H
#include <Type.h>

typedef volatile dword Mutex;

// On single-processor systems, anything that
// happens in one instruction is always atomic

// Mutex locks are useful because they avoid critical sections
// where they are not needed. E.g. ISA DMA, CMOS
// They ensure that different codes use a resource in order
// so drivers need to aquire and release locks

#define AcquireLock(address_lock) while (!__sync_bool_compare_and_swap(lock, 0, 1)) {}
#define ReleaseLock(address_lock) *address_lock = 0;

#endif /* MUTEX_H */
