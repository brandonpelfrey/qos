#include <stdint.h>

#include "Memory.h"
#include "Terminal.h"
#include "libc.h"
#include "multiboot.h"

extern "C" void kernel_main(multiboot_info_t *mbd, unsigned magic) {
  terminal::initialize();
  memory::initialize(mbd);

  printf("Reached end of kernel\n");
}