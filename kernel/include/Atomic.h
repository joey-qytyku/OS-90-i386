#ifndef MUTEX_H
#include <Type.h>

typedef volatile dword Mutex;

// On single-processor systems, anything that
// happens in one instruction or multiple with
// interrupts disabled should always be atomic


// Mutex locks are useful because they avoid critical sections
// where they are not needed. E.g. ISA DMA, CMOS
// They ensure that different components use a resource in order

#define AcquireLock(address_lock) while (!__sync_bool_compare_and_swap(address_lock, 0, 1)) {}
#define ReleaseLock(address_lock) *address_lock = 0;

// Compiler memory fence
#define MemFence() __asm__ __volatile__ ("":::"memory")

#endif /* MUTEX_H */
