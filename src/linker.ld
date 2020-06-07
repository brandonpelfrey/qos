/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
OUTPUT_FORMAT("elf64-x86-64");
ENTRY(__boot);
SEARCH_DIR(build);

SECTIONS
{
	. = 0x7c00;
	.bootloader_stage1 : {
		bootloader/boot1.o(.text);
	}

	.bootloader_signature 0x7DFE : {
		SHORT(0xaa55);
	}

	.bootloader_stage2 : {
		bootloader/boot2.o(.text);
	}

	.kernel : {
		kernel_*.o(.text);
	}

	.data : SUBALIGN(2) {
		*(.data);
		*(.rodata*);
	}

	.bss : SUBALIGN(4) {
			__bss_start = .;
			*(COMMON);
			*(.bss)
			. = ALIGN(4);
			__bss_end = .;
	}

	/DISCARD/ : {
		*(.eh_frame);
	}
}