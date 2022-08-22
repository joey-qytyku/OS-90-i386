#ifndef MEMORY_H
#define MEMORY_H

#define PG_SHIFT 12

// walkpgdir from xv6?

#define PG_DIRTY 1<<6
#define PG_ACC   1<<5 /* The page table/dir was used */
#define PG_CHDIS 1<<4
#define PG_WRTH  1<<3
#define PG_USER  1<<2
#define PG_RDWR  1<<1
#define PG_PRES  1<<0

#define PAGE dword;

static inline void i486_invlpg(PAGE addr)
{
    __asm__ volatile ("invlpg (%0)"::"r"(addr) :"memory");
}

#endif /* MEMORY_H */

