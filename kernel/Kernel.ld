/*
     This file is part of OS/90.

    OS/90 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

    OS/90 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.
*/

OUTPUT_FORMAT(binary) /* IDT direct binary doesn't' work? */

SECTIONS
{
	. = 0xC0000000;
	.text : AT(LKR_STARTTEXT)
	{
		LKR_STARTTEXT = .;
		*(.init)
		*(.text)
		. = ALIGN(4K); /* Has to be in here */
		LKR_ENDTEXT = .;
	}

	.data : AT(LKR_STARTTEXT + (LKR_STARTDATA - LKR_STARTTEXT))
	{
		LKR_STARTDATA = .;
		*(.data)
		. = ALIGN(4K);
		LKR_ENDDATA = .;
	}

	.bss : AT(LKR_STARTTEXT + (LKR_STARTBSS - LKR_STARTDATA))
	{
		LKR_STARTBSS = .;
		*(.bss)
		. = ALIGN(4K);
		LKR_END = .;
	}
}
