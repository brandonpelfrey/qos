/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
OUTPUT_FORMAT("elf64-x86-64");
ENTRY(__boot);

SECTIONS
{
	. = 0x7c00;
	.text1 : {
		boot1.o(.text);
	}

	.bootloader_signature 0x7DFE : {
		SHORT(0xaa55);
	}

	.text2 : {
		boot2.o(.text);
		*(.text);
	}

	.data : SUBALIGN(2) {
		*(.data);
		*(.rodata*);
	}

	/* Place the uninitialised data in the area after our bootloader
     * The BIOS only reads the 512 bytes before this into memory */
    .bss : SUBALIGN(4) {
        __bss_start = .;
        *(COMMON);
        *(.bss)
        . = ALIGN(4);
        __bss_end = .;
    }
    __bss_sizeb = SIZEOF(.bss);
}