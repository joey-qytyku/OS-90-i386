#include <stdarg.h>
#include <Type.h>

#define MAX_STR_LENGTH_OF_UINT32 10

// It is the output driver's responsibility to handle ascii sequences
// Logf sends the character when it is not a format escape
//
typedef VOID (*OUTPUT_DRIVER)(BYTE);

static BYTE printfmt_buffer[MAX_STR_LENGTH_OF_UINT32 + 1];

// GCC builtin always refers to the glibc function for some reason
// so I have to implement it manually, inspired by the BSD implementation
DWORD StrLen(PBYTE s1)
{
    PBYTE s2;
    for (s2 = s1; *s2; s2++);
    return s2-s1;
}

//
// The following function are probably slow, but there does not
// seem to be a perfect way of doing it, besides this one I found?
// https://www.quora.com/What-are-the-most-obscure-useless-x86-assembly-instructions?
//

VOID APICALL Hex32ToString(DWORD value,  PBYTE obuffer)
{
}

VOID APICALL Uint32ToString(DWORD value, PBYTE obuffer)
{
    DWORD digit, digit_divisor;
    DWORD buff_off = MAX_STR_LENGTH_OF_UINT32 - 2;
    DWORD i;

    // Clear buffer by setting all chars to ascii NUL
    // so that they are not printed
    C_memset(obuffer, '\0', MAX_STR_LENGTH_OF_UINT32);

    // The following loops through each  *digit
    // an then copies them to the buffer in reverse order
    // the integer digit is then converted to a character


    // It looks complicated, but dividing by 1, 10, 100, etc.
    // is like bitwise shifting but for decimal digits, not binary.
    // Modulus is like an AND operation, getting the remainder
    // or "offset" in CS terms. Together, it is a sort of shift/and loop

    for (i=0, digit_divisor=1; i<MAX_STR_LENGTH_OF_UINT32; i++)
    {
        digit = (value / digit_divisor) % 10;
        obuffer[buff_off] = '0' + digit;

        // ENDING STATEMENTS
        digit_divisor *= 10;
        buff_off--;
    }
}

// @x - Hex32
// @i - Int32
// @s - string (# not valid)
// # for signed
// Example:
//  KeLogf(LptDebug, "Value = @d\n\t", value)
//
VOID APICALL KeLogf(OUTPUT_DRIVER od, IMUSTR restrict fmt, ...)
{
    va_list ap;
    WORD i;
    BYTE ch;
    BOOL is_signed;

    va_start(ap,i);

    for (i=0, ch=fmt[i]; ch != 0; i++)
    {
        if (ch == '@') {is_signed = 0;}
        else if  ('#') {is_signed = 1;}
        else           {od(ch); continue;}

        // The following runs if this is a format character
        i+=2; // Skip the format characters
    
        switch (fmt[i-1])
        {

        // Print hexadecimal, sign is ignored
        case 'x':
            va_arg(ap, DWORD);
        break;

        // Print integer, signed or unsigned format
        case 'i':
        break;

        case 's':
        break;

        case '#':
        case '@': od(fmt[i-1]);
        break;

        default:
            goto FailSilent;
        }
    }

    FailSilent:
    // This function must end with va_end
    va_end(ap);
}
