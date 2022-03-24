OUTPUT_FORMAT("elf32-i386")

SECTIONS
{
	__start = 1M;

	.text ALIGN(4K): AT(__start)
	{
		*(.init)
		__start = 0xC0000000;
		*(.text)
	}
	__end_text = .;

	.data ALIGN(4K): AT(__end_text)
	{
		*(.data)
	}
	__end_data = .;

	.bss ALIGN(4K): AT(__end_data)
	{
		*(.bss)
	}
	. = ALIGN(4K);
	__end = .;
}
