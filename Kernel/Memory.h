#pragma once

#include "multiboot.h"

namespace memory {
  void initialize(multiboot_info_t *mbd);
  void* kmalloc();
  void kfree();
}