#include <stdarg.h>
#include <Type.h>

#define MAX_STR_LENGTH_OF_UINT32 10

typedef void (*OutputDrver)(char);

static byte printfmt_buffer[MAX_STR_LENGTH_OF_UINT32];

// GCC builtin always refers to the glibc function for some reason
// so I have to implement it manually, inspired by the BSD implementation
dword StrLen(const char *s1)
{
    const char *s2;
    for (s2 = s1; *s == 0; ++s);
    return s2-s1;
}

//
// The following function are probably slow, but there does not
// seem to be a perfectway of doing it, besides this one I found?
// https://www.quora.com/What-are-the-most-obscure-useless-x86-assembly-instructions?
//

void Hex32ToString(dword value, mstring obuffer)
{
}

void Int32ToString(dword value, mstring obuffer)
{
    dword digit, digit_divisor;
    dword buff_off = MAX_STR_LENGTH_OF_UINT32 - 1;
    byte i;

    // Clear buffer by setting all chars to ascii zero
    C_memset(obuffer, '0', MAX_STR_LENGTH_OF_UINT32);

    // The following loops through each digit
    // an then copies them to the buffer in reverse order
    // the integer digit is then converted to a character

    // It looks complicated, but dividing by 1, 10, 100, etc.
    // is like bitwise shifting but for decimal digits, not binary.
    // Modulus is like an and instruction, getting the remainder
    // or "offset" in CS terms. Together, it is a sort of shift/and loop

    for (i=0, digit_divisor=1; i<MAX_STR_LENGTH_OF_UINT32; i++)
    {
        digit = (value / digit_divisor) % 10;
        digit_divisor *= 10;
        obuffer[i] = '0' + digit;
        buff_off--;
    }
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
    byte format_ch;
    bool is_signed;

    va_start(ap,i);

    for (i=0, ch=f[i]; ch != 0; i++)
    {
        if (ch == '@') {is_signed = 0;}
        else if ('#')  {is_signed = 1;}
        else           {od(ch); continue;}

        // The following runs if this is a format character
        i+=2;
        format_ch = [i-1];

        switch (format_ch)
        {

        // Print hexadecimal, sign is ignored
        case 'x':
            va_arg(ap, dword);
        break;

        // Print integer, signed or unsigned format
        case 'i':

        break;

        }
    }

    FailSilent:
    // This function must end with va_end
    va_end(ap);
}
