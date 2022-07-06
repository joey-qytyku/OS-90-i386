#ifndef IO_H
#define IO_H

static inline void outb(word port, byte val)
{
    asm volatile ("outb %0, %1": :"a"(val), "Nd"(port));
}

static inline byte inb(word port)
{
    byte ret;
    asm volatile ("inb %1, %0" :"=a"(ret) :"Nd"(port) );
    return ret;
}

static inline void rep_insb(pvoid mem, dword count, word port)
{__asm__ volatile ("rep insb"::"esi"(mem),"ecx"(count),"dx"(port) :"esi","edi","dx");
}

static inline void rep_outsb(pvoid mem, dword count, word port)
{__asm__ volatile ("rep outsb"::"esi"(mem),"ecx"(count),"dx"(port) :"esi","edi","dx");
}

static inline void rep_insw(pvoid mem, dword count, word port)
{__asm__ volatile ("rep insw"::"esi"(mem),"ecx"(count),"dx"(port) :"esi","edi","dx");
}

static inline void rep_outsw(pvoid mem, dword count, word port)
{__asm__ volatile ("rep outsw"::"esi"(mem),"ecx"(count),"dx"(port) :"esi","edi","dx");
}

#endif IO_H
