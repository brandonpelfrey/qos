#include <stdarg.h>
#include "Terminal.h"
#include "libc.h"

constexpr u32 strlen(const char* str) 
{
	auto len = 0;
	while (str[len])
		len++;
	return len;
}

i32 memcpy(void* destination, void* source, u32 length) {
	char *d=(char*)destination;
	char *s=(char*)source;

	for(u32 i=0; i<length; ++i)
		*d++ = *s++;
		
	return length;
}

void* memset(void* start, u8 value, u32 length) {
	u8 *ptr = (u8*)start;
	for(u32 i=0; i<length; ++i)
		*ptr++ = value;
	return ptr;
}

void putc(const char c) {
	terminal::putc(c);
}

void puts(const char* str) {
	terminal::write(str);
}

const char* HEX_DIGITS = "0123456789ABCDEF";

template<typename Printer>
u32 printf_d(int value, u32 base, Printer printer) {
	
	u32 written = 0;
	if(base == 10 && value < 0) {
		value = -value;
		printer('-');
		written += 1;
	}
	else if(base == 16) {
		printer('0');
		printer('x');
		written += 2;
	}

	if(value == 0) {
		printer('0');
		written += 1;
		return written;
	}
	
	// Get the digits in reverse order, then walk back to the beginning of the array
	char buff[32];
	char *buff_ptr = &buff[0];

	while(value) {
		*buff_ptr = HEX_DIGITS[value % base];
		buff_ptr++;
		value /= base;
	}

	// TODO : Zero-padding here.

	while(buff_ptr != buff) {
		buff_ptr--;
		printer(*buff_ptr);
		written += 1;
	}

	return written;
}

i32 printf(const char* format, ...) {
	va_list parameters;
	va_start(parameters, format);

	i32 written = 0;
	while(*format != '\0') {
		if(*format == '%') {

			// TODO : Formatting, padding, etc. (%04d, %-4s, etc.)

			if(format[1] == 'd' || format[1] == 'x') {
				const u32 base = (format[1] == 'd') ? 10 : 16;
				written += printf_d(va_arg(parameters, int), base, [](const char c) { putc(c); });
				format += 2;
			} 
			
			else if(format[1] == 's') {
				const char* str = va_arg(parameters, const char*);
				puts(str);
				written += strlen(str);
				format += 2;
			} 
			
		} else {
			written ++;
			putc(*format++);
		}
	}

	va_end(parameters);
	return written;
}
 