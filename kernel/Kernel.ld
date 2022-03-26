OUTPUT_FORMAT(binary)

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
