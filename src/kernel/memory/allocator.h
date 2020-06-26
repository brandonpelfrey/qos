#pragma once

#include "memory_map.h"

namespace Memory
{
void init(SMAP_entry* smap_data);
void* kmalloc(size_t size);
void kfree(void* ptr);
}

using Memory::kfree;
using Memory::kmalloc;
