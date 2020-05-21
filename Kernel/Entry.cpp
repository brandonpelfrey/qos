#include <stdint.h>
#include "Terminal.h"
#include "Memory.h"
#include "multiboot.h"
#include "libc.h"

extern "C"
void kernel_main(multiboot_info_t *mbd, unsigned magic)
{
	terminal::initialize();
	memory::initialize(mbd);
	

	printf("Reached end of kernel\n");
}