#ifndef IO_H
#define IO_H

static inline void outb(WORD port, BYTE val)
{
    asm volatile ("outb %0, %1": :"a"(val), "Nd"(port));
}

static inline BYTE inb(WORD port)
{
    BYTE ret;
    asm volatile ("inb %1, %0" :"=a"(ret) :"Nd"(port) );
    return ret;
}

static inline void rep_insb(PVOID mem, DWORD count, WORD port)
{__asm__ volatile ("rep insb"::"esi"(mem),"ecx"(count),"dx"(port) :"esi","edi","dx");
}

static inline void rep_outsb(PVOID mem, DWORD count, WORD port)
{__asm__ volatile ("rep outsb"::"esi"(mem),"ecx"(count),"dx"(port) :"esi","edi","dx");
}

static inline void rep_insw(PVOID mem, DWORD count, WORD port)
{__asm__ volatile ("rep insw"::"esi"(mem),"ecx"(count),"dx"(port) :"esi","edi","dx");
}

static inline void rep_outsw(PVOID mem, DWORD count, WORD port)
{__asm__ volatile ("rep outsw"::"esi"(mem),"ecx"(count),"dx"(port) :"esi","edi","dx");
}

#endif /* IO_H */
