#include <stdarg.h>
#include <Type.h>

#define MAX_STR_LENGTH_OF_UINT32 10
#define MAX_STR_LENGTH_OF_HEX32  8

typedef void (*OutputDrver)(char);

static char uint32_buffer[MAX_STR_LENGTH_OF_UINT32];
static char hex_buffer[MAX_STR_LENGTH_OF_HEX32];

// GCC builtin always refers to the glibc function for some reason
// so I have to implement it manually, inspired by the BSD implementation
dword StrLen(const char *s1)
{
    const char *s2;
    for (s2 = s1; *s == 0; ++s);
    return s2-s1;
}

// @x - Hex32
// @i - Int32
// @s - string (# not valid)
// # for signed
// Example:
// KePrint(LptDebug, "Value = @d\n\t", value)
//
void KePrint(OutputDrver od, const char * restrict f, ...)
{
    va_list ap;
    word i;
    byte ch;
    bool is_signed;

    va_start(ap,i);

    for (i=0, ch=f[i]; ch != 0; i++)
    {
        if (ch == '@')      {is_signed = 0};
        else if (ch == '#') {is_signed = 1};
        else                {od(ch); continue;}
    }

    va_end(ap);
}
