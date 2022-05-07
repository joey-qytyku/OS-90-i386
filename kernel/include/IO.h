#include <Type.h>

static inline byte inb(short port)
{
	byte ret;
	__asm__ volatile ("inb %1, %0":"=a"(ret) : "Nd"(port));
	return ret;
}

// Port to memory
static inline void rep_insb(void *mem, dword count, word port)
{
	__asm__ volatile ("rep insb":"=esi"(mem),"=ecx"(count),"=dx"(port));
}

// Memory to port
static inline void rep_outsb(void *mem, dword count, word port)
{
	__asm__ volatile ("rep outsb":"=esi"(mem),"=ecx"(count),"=dx"(port));
}

// Port to memory
static inline void rep_insw(void *mem, dword count, word port)
{
	__asm__ volatile ("rep insw":"=esi"(mem),"=ecx"(count),"=dx"(port));
}

// Memory to port
static inline void rep_outsw(void *mem, dword count, word port)
{
	__asm__ volatile ("rep outsw":"=esi"(mem),"=ecx"(count),"=dx"(port));
}

