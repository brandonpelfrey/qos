OUTPUT_FORMAT(elf64-x86-64);
ENTRY(__kernel_main);
SEARCH_DIR(build);

SECTIONS
{
	. = 0xFFFFFFFF80000000;
	KERNEL_START = .;
	.bootstrap : { boot3.o(.text); }

	.kernel_code : AT(.kernel - ADDR(KERNEL_START)) { kernel_*.o(.text); }
	.kernel_data : AT(.kernel_data - ADDR(KERNEL_START)) { kernel_*.o(.data); kernel_*.o(.rodata); }
	.kernel_bss : AT(.kernel_bss - ADDR(KERNEL_START)) { kernel_*.o(.bss); }

	/DISCARD/ : {
		*(.eh_frame);
	}
}