target remote localhost:1234
add-symbol-file build/bootloader-linked.o -s .bootloader_stage1 0x7c00
add-symbol-file build/kernel-linked.o 0xFFFFFFFF80000000
b LongModeStart
c