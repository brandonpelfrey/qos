#pragma once

#include <stdarg.h>
#include <types.h>

constexpr u32 strlen(const char* str);
void puts(const char* str);
void putc(const char c);
int printf(const char* format, ...);
int printf_valist(const char* format, va_list parameters);
void memcpy(u8* destination, u8* source, u64 size);
void* memset(void* start, u8 value, u32 length);