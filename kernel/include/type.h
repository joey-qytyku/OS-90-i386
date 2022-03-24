#include <stddef.h>

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

extern char *strtok(char *str, const char *delim);
