#include "Memory.h"
#include "libc.h"
#include "types.h"
#include "multiboot.h"

using mmap_entry_t = multiboot_mmap_entry;



void memory::initialize(multiboot_info_t *mbd) {
  if(!(mbd->flags & 1)) {
		puts("Multiboot info does not seem to be setup.\n");		
		return;
	}

  printf("Initializing memory map from GRUB Multiboot data...\n");

  mmap_entry_t *entry = (mmap_entry_t*)(mbd->mmap_addr);
  while((u32)entry < mbd->mmap_addr + mbd->mmap_length) {

    printf("MMapEntry : (Type=%d, Addr=%x, Length=%x, Size=%x)\n", entry->type, entry->addr, entry->len, entry->size);

    entry = (mmap_entry_t*)((u32)entry + entry->size + sizeof(entry->size));
  }
}

void* memory::kmalloc() {
  // TODO
  puts("kmalloc called\n");
  return nullptr;
}

void memory::kfree() {
  puts("KFree called\n");
}

