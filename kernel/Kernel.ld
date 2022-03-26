OUTPUT_FORMAT("elf32-i386")

SECTIONS
{
	. = 0xC0000000;
	LKR_STARTTEXT = .;

	.text ALIGN(4K): AT(.)
	{
		*(.text)
	}
	LKR_ENDTEXT = .;

	LKR_STARTDATA = .;
	.data ALIGN(4K): AT(LKR_STARTDATA)
	{
		*(.data)
	}
	LKR_ENDDATA = .;

	LKR_STARTBSS = .;
	.bss ALIGN(4K): AT(LKR_STARTBSS)
	{
		*(.bss)
	}
	. = ALIGN(4K);
	LKR_END = .;
}
