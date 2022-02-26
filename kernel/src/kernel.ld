OUTPUT_FORMAT("binary")
SECTIONS
{
	__start = 0x1000;

	.text AT(__start):
	{
		startk.o(.startk)
		*(.text)
		. = ALIGN(4K)
	}

	.data AT(__start + (__data - start)):
	{
		__data = .;
		*(.data)
		. = ALIGN(4K);
	}

	.bss AT(__start + (__bss - __text)):
	{
		__bss = .
		*(.bss)
		. = ALIGN(4K); /* Needs it too */
	}
	__end = .
}
