#ifndef _LINKER_H
#define _LINKER_H

// The kernel is loaded at 1/3 of the virtual address space.
// 1M is identity mapped to the start of memory

#define HIGHER_HALF_START 0xC0000000

/* Virtual to physiscal */
#define v2p(a) (a-0xC0000000)

/* In C, get these linker symbols using address-of operator (&) */
extern int LKR_STARTTEXT;
extern int LKR_ENDTEXT;
extern int LKR_STARTDATA;
extern int LKR_ENDDATA;
extern int LKR_STARTBSS;
extern int LKR_END;

#endif
