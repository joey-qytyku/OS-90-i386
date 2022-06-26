#ifndef TYPE_H
#define TYPE_H

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;

typedef char  sbyte;
typedef short sword;
typedef long  sdword;

typedef void*	  pvoid;
typedef word*     pword;
typedef byte*     pbyte;
typedef dword*    pdword;

typedef sword*     psword;
typedef sbyte*     psbyte;
typedef sdword*    psdword;

typedef sdword Handle;
typedef sdword Status;
typedef byte bool;

#define NULL ((void*)(0))

/* Volatile variables can change at any time without the
 * compiler being aware. This applies to ISRs and drivers
 * because they are unpredictable
 */
#define INTVAR volatile /* Used by interrupt handler */
#define DRVMUT volatile /* Driver can modify */

// Packed structure
#define __PACKED   __attribute__( (packed) )
#define __ALIGN(x) __attribute__( (aligned(x)) )

/*  The timer interrupt uses fixed point rather than FPU
 *  This is because milisecond precision time counting
 *  is supposedly very inaccurate
**/

struct FixedPointNumber { long fractional, whole; };

/* Builtin functions use inline x86 string operations
 * making them way faster that doing it in C.
 * string operand size can also be deduced by the compiler
*/

#if __GNUC__

static inline void *C_memcpy(void *d, void *s, dword c)
{
	return __builtin_memcpy(d, s, c);
}

static inline void *C_memmove(void *d, void *s, dword c)
{
	return __builtin_memmove(d, s, c);
}

static inline int C_strcmp(char *s1, char *s2)
{
	return __builtin_strcmp(s1, s2);
}

static inline void C_memset(void *a, unsigned int val, int count)
{
    __builtin_memset(a,val,count);
}

#endif /* __GNUC__*/

#endif /* TYPE_H */
