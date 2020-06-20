OUTPUT_FORMAT(elf64-x86-64);
ENTRY(__boot);
SEARCH_DIR(build);

SECTIONS
{
  /* First sector simply loads the kernel from disk. */
	. = 0x7c00;
	.bootloader_stage1 : { bootloader/boot1.o(.text); bootloader/boot1.o(.bss); }
	.bootloader_signature 0x7DFE : { SHORT(0xaa55); }

	/* Setup Virtual Memory / Paging / Jump to high memory. */
	. = 0x7e00;
	.bootloader_stage2 0x7E00 : { bootloader/boot2.o(.text); }
	.bootloader_bss    .      : { bootloader/boot2.o(.bss);	}

	/* Pad up to 4K. */
	.bootloader_padding : AT(0x7c00 + 4096 - 2) { SHORT(0x0000); }
}