#pragma once
#include "types.h"

namespace vm {
struct SMAP_entry {
  uint32_t BaseL;  // base address uint64_t
  uint32_t BaseH;
  uint32_t LengthL;  // length uint64_t
  uint32_t LengthH;
  uint32_t Type;  // entry Type
  uint32_t ACPI;  // extended
} __attribute__((packed));

void initialize(SMAP_entry* smap_data);
}  // namespace vm