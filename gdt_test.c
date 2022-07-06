#include <stdio.h>
#include <stdint.h>

typedef struct __PACKED
{
    __UINT16_TYPE__ limit, base0;
    __UINT8_TYPE__  base1;
    __UINT8_TYPE__  access;
    __UINT8_TYPE__  limit_gr;
    __UINT8_TYPE__  base2;
}Gdesc;

#define MAKE_FLAT_DESCR(dpl, type) ( (0xFFFFL<<48) | (0x8000L<<32) | ((dpl<<5 | type) << 24) )

unsigned long i = 0x00890000002068UL | (0xCAFEul << 16) | (0xBAul << 32) | (0xBEul) << 56;

int main()
{
    Gdesc *g = (Gdesc*)&i;
    printf("%lx", g->base2);
}
