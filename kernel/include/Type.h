#ifndef TYPE_H
#define TYPE_H

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;

typedef char	sbyte;
typedef short	sword;
typedef long	sdword;
typedef	long long sqword;
typedef	unsigned long long qword;


typedef void*	  pvoid;
typedef word*     pword;
typedef byte*     pbyte;
typedef dword*    pdword;
typedef qword*	  pqword;

typedef sdword Handle;
typedef sdword Status;
typedef byte bool;

#define BIT_IS_SET(num, bit) ((num & 1<<bit)>0)

// The BSWAP instruction is only supported by i486 and above
// but this is only a macro. I figured this out myself :)
#define BYTESWAP(value) ((value & 0xFF) << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000)>>8) | ((value & 0xFF000000) >> 24)

#define NULL ((void*)(0))

//
// Volatile variables can change at any time without the
// compiler being aware. This applies to ISRs and drivers
// because they are unpredictable and also assembly code that
// modifies a C variable
//
#define INTVAR volatile /* Used by interrupt handler */
#define DRVMUT volatile /* Driver can modify */
#define ASMVAR volatile /* Assembly code may modify */
#define DONT_DELETE     /* Do not optimize away variable */

#ifdef __PROGRAM_IS_DRIVER
#define KRNMUT volatile // Kernel may modify
#endif

/* Builtin functions use inline x86 string operations
 * making them way faster that doing it in C.
 * string operand size can also be deduced by the compiler
*/

#if __GNUC__

#define __PACKED   __attribute__( (packed) )
#define __ALIGN(x) __attribute__( (aligned(x)) )

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
