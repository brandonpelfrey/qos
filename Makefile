.PHONY: doit
doit:
	nasm -f elf64 boot1.s -o boot1.o
	nasm -f elf64 boot2.s -o boot2.o
	g++ -std=c++17 -mno-red-zone -march=x86-64 -c test.cpp -o kernel.o -ffreestanding -g -O0 -Wall -fno-exceptions -fno-rtti
	/usr/local/Cellar/x86_64-elf-binutils/2.34/bin/x86_64-elf-ld -T linker.ld boot1.o boot2.o kernel.o -o image.bin -melf_x86_64
	/usr/local/Cellar/x86_64-elf-binutils/2.34/bin/x86_64-elf-objcopy -O binary image.bin bootable.bin
	