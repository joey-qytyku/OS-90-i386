#include <stddef.h>
#include <stdint.h>

typedef uint16_t  word;
typedef uint8_t   byte;
typedef uint32_t  dword;

typedef int16_t  sword;
typedef int8_t   sbyte;
typedef int32_t  sdword;

static inline void *memcpy(void *d, void *s, size_t c)
{
	return __builtin_memcpy(d, s, c);
}

static inline void *memmove(void *d, void *s, size_t c)
{
	return __builtin_memmove(d, s, c);
}

static inline int strcmp(char *s1, char *s2)
{
	return __builtin_strcmp(s1, s2);
}
