#pragma once

#include "types.h"

constexpr u32 strlen(const char* str);
void puts(const char* str);
void putc(const char c);
i32 printf(const char* format, ...);
i32 memcpy(void* destination, void* source, u32 length);
void* memset(void* start, u8 value, u32 length);
