
#include "terminal.h"
#include "libc.h"

struct SMAP_entry {
	uint32_t BaseL; // base address uint64_t
	uint32_t BaseH;
	uint32_t LengthL; // length uint64_t
	uint32_t LengthH;
	uint32_t Type; // entry Type
	uint32_t ACPI; // extended
}__attribute__((packed));

extern "C"
int __kernel_main(u8 smap_entries_count, SMAP_entry* smap_data) {
  terminal::initialize();

  printf("%d Memory Map (SMAP) Entries @ 0x%lx\n", smap_entries_count, smap_data);
  for(int i=0; i<smap_entries_count; ++i) {
    const auto& entry(smap_data[i]);
    printf(" - Start  0x%08x%08x Length %08x%08x Type %d\n", entry.BaseH, entry.BaseL, entry.LengthH, entry.LengthL, entry.Type);
  }
  
  while (1)
    ;

  return 0;
}
