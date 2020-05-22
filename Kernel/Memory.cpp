#include "Memory.h"

#include "libc.h"
#include "multiboot.h"
#include "types.h"

using mmap_entry_t = multiboot_mmap_entry;

void memory::initialize(multiboot_info_t *mbd) {
  if (!(mbd->flags & 1)) {
    puts("Multiboot info does not seem to be setup.\n");
    return;
  }

  printf("Initializing memory map from GRUB Multiboot data...\n");

  mmap_entry_t *entry = (mmap_entry_t *)(mbd->mmap_addr);
  while ((u32)entry < mbd->mmap_addr + mbd->mmap_length) {
    if (entry->type == 1) {
      const u32 kbytes = entry->len / 1024;
      const u32 mbytes = entry->len / 1024 / 1024;

      printf("MemoryMap Entry : (Type=%d, Addr=%9lx, Length=%9lx (%3d.%d MB) )\n", entry->type, entry->addr, entry->len,
             mbytes, kbytes % 1024);
    }

    entry = (mmap_entry_t *)((u32)entry + entry->size + sizeof(entry->size));
  }
}

void *memory::kmalloc() {
  // TODO
  puts("kmalloc called\n");
  return nullptr;
}

void memory::kfree() { puts("KFree called\n"); }
