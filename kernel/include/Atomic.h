/*
     This file is part of OS/90.

    OS/90 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

    OS/90 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with OS/90. If not, see <ttps://www.gnu.org/licenses/>.
*/

#ifndef MUTEX_H
#define MUTEX_H

#include <Platform/8259.h>
#include <Type.h>

typedef volatile DWORD MUTEX;

// On single-processor systems, anything that
// happens in one instruction or multiple with
// interrupts disabled should always be atomic

// Mutex locks are useful because they avoid critical sections
// where they are not needed. E.g. ISA DMA, CMOS
// They ensure that different components use a resource in order

#define AcquireLock(address_lock) while (!__sync_bool_compare_and_swap  (address_lock, 0, 1)) {}

#define ReleaseLock(address_lock) *address_lock = 0;

// Compiler memory fence. This OS is for single processor systems, so a compiler memory fence
// on a non-preemptible kernel is a true fence.
#define MemFence() __asm__ volatile ("":::"memory")

//
// Is this right?
// What if the interrupts are already off?
//
#define EnterCriticalSecttion() IntsOff()
#define ExitCriticalSection()   IntsOn()

#endif /* MUTEX_H */
