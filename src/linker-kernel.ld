OUTPUT_FORMAT(elf64-x86-64);
ENTRY(__kernel_main);
SEARCH_DIR(build);

SECTIONS
{
	. = 0xFFFFFFFF80000000;
	KERNEL_START = .;
	/* Note: Strange behavior (bug?) in LD. This path needs to exactly match, or the EXCLUDE_FILE logic will not work.. */
	.bootstrap : { build/kernel_boot3_s.o(.text); }

	.kernel_code : AT(.kernel - ADDR(KERNEL_START)) { EXCLUDE_FILE(kernel_boot3_s.o) kernel_*.o(.text); }
	.kernel_data : AT(.kernel_data - ADDR(KERNEL_START)) { kernel_*.o(.data); kernel_*.o(.rodata); }
	.kernel_bss : AT(.kernel_bss - ADDR(KERNEL_START)) { kernel_*.o(.bss); }

	/DISCARD/ : {
		*(.eh_frame);
	}
}