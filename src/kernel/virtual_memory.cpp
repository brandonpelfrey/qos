#include <libc.h>
#include <utils/utils.h>
#include <virtual_memory.h>
#include <logging.h>

namespace vm {

static klog_spec VM { .module_name="VM", .text_color=terminal::VGA_COLOR_LIGHT_CYAN };

void cr3_write(u64 newval) { asm volatile("mov %0, %%cr3" : : "r"(newval) : "memory"); }

void setup_paging() {
  // Modify the PML4 table from the bootloader a little to also allow indexing into main RAM
  u64* P4 = (u64*)0x1000;
  P4[511] = 0x5000 | 3;

  u64* P3 = (u64*)0x5000;
  for (int i = 0; i < 512; ++i) P3[i] = 0;
  P3[510] = 0x6000 | 3;
  // TODO : We're mapping only one of the two GiB. Need to fill in entry 512.

  u64* P2 = (u64*)0x6000;
  for (int i = 0; i < 512; ++i) {
    const u64 P2_LARGE_PAGE_SIZE = (2 * 1024 * 1024);  // 2MB
    const u64 offset = (size_t)0x200000 + i * P2_LARGE_PAGE_SIZE;
    P2[i] = offset | 128 | 3;
  }
  cr3_write(0x1000);
}

SMAP_entry* __smap_array; // Ends when type == 0xFFFFFFFF
SMAP_entry* __conventional_memory_root;

SMAP_entry* get_largest_conventional_memory_region(SMAP_entry* smap_data) {
  SMAP_entry* largest_entry = nullptr;
  u64 largest_entry_size = 0;

  SMAP_entry* entry = smap_data;
  while (entry->Type != 0xFFFFFFFF) {
    if (entry->Type == 1) {
      u64 region_size = ((u64)entry->LengthH << 32) | (u64)entry->LengthL;
      if (region_size > largest_entry_size) {
        largest_entry_size = region_size;
        largest_entry = entry;
      }
    }
    entry ++;
  }

  if (largest_entry == nullptr) {
    kpanic("No Type-1 memory regions found in SMAP.");
  }

  return largest_entry;
}

void parse_physical_memory(SMAP_entry* smap_data) {
  // Find a single main memory segment
  __smap_array = smap_data;
  __conventional_memory_root = get_largest_conventional_memory_region(smap_data);

  size_t region_size = ((size_t)__conventional_memory_root->LengthH << 32) | (size_t)__conventional_memory_root->LengthL;
  auto region_size_kib = region_size / 1024;
  kprintf(VM, "Found %d.%d MiB usable RAM @ Physical address 0x%08x%08x\n", region_size_kib / 1024, region_size_kib % 1024,
         __conventional_memory_root->BaseH, __conventional_memory_root->BaseL);
}

void initialize(SMAP_entry* smap_data) {
  kprintf(VM, "Setting up virtual memory\n");

  parse_physical_memory(smap_data);
  setup_paging();
}

}  // namespace vm