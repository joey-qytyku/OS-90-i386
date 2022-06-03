#ifndef TYPE_H
#define TYPE_H

#include <stddef.h>
#include <stdint.h>

typedef uint16_t  word;
typedef uint8_t   byte;
typedef uint32_t  dword;

typedef int16_t   sword;
typedef int8_t    sbyte;
typedef int32_t   sdword;

typedef void*	  pvoid;
typedef word*     pword;
typedef byte*     pbyte;
typedef dword*    pdword


/*  The timer interrupt uses fixed point rather than FPU
 *  This is because milisecond precision time counting
 *  is supposedly very inaccurate
*/
struct FixedPointNumber { long whole, fractional; };

static inline void *C_memcpy(void *d, void *s, size_t c)
{
	return __builtin_memcpy(d, s, c);
}

static inline void *C_memmove(void *d, void *s, size_t c)
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

#endif
