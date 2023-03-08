/*
     This file is part of OS/90.

    OS/90 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

    OS/90 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with OS/90. If not, see <ttps://www.gnu.org/licenses/>.
*/

#include <stdarg.h>
#include <Type.h>
#include <V86.h> /* For calling BIOS functions */
#include <Debug.h>

#define MAX_STR_LENGTH_OF_UINT32 10

// GCC builtin always refers to the glibc function for some reason
// so I have to implement it manually, inspired by the BSD implementation
DWORD StrLen(PBYTE s1)
{
    PBYTE s2;
    for (s2 = s1; *s2; s2++);
    return s2-s1;
}

//
// The following functions are probably slow, but there does not
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
    // or "offset" in ComSci terms. Together, it is a sort of shift/and loop

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
//
// It is the output driver's responsibility to handle ascii sequences
// Logf sends the character when it is not a format escape
//
VOID APICALL KeLogf(OUTPUT_DRIVER od, IMUSTR restrict fmt, ...)
{

    BYTE printfmt_buffer[MAX_STR_LENGTH_OF_UINT32 + 1];

    va_list ap;
    WORD i;
    BYTE ch;
    BOOL is_signed;

    va_start(ap, fmt);

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

VOID _KernelPutchar(BYTE ch)
{
    TRAP_FRAME regparm = { .regs.eax = (0xE << 8) | ch};
    ScVirtual86_Int(&regparm, 0x10);
}

VOID FatalError(DWORD error_code)
{
    TRAP_FRAME regparm = { 0 };
    volatile PBYTE text_attrib = (PBYTE)0xB8000;

    ScOnErrorDetatchLinks();

    // Switch to text mode 3
    regparm.regs.eax = 3;
    ScVirtual86_Int(&regparm, 0x10);

    // Print the message
    KeLogf(_KernelPutchar,
    "The OS/90 kernel has encounterd a fatal error.\n\t"
    "Please restart your computer.\n\t"
    "Error code: @x", error_code
    );

    // Make the screen blue :)
    for (WORD i = 0; i<4000;i+=2)
        text_attrib[i] = 0x17;
}
