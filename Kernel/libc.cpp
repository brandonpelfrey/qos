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

////////////////

struct printf_details {
	int min_field_width;
	bool is_zero_padded;
	bool is_long;
};

const char* HEX_DIGITS = "0123456789ABCDEF";

template<typename Printer, typename Num>
void printf_dux(int value, Num base, const printf_details& details, Printer printer) {
	
	// TODO : Fix duplication, better state machine

	if(base == 10 && value < 0) {
		value = -value;
		printer('-');
	}

	if(value == 0) {
		for(u32 i=1; i<details.min_field_width; ++i)
			printer(details.is_zero_padded ? '0' : ' ');
		printer('0');
		return;
	}
	
	// Enter digits into a buffer from end to beginning
	static char buff[32];
	char *buff_ptr = &buff[31];
	i32 chars_written=0;

	while(value) {
		--buff_ptr;
		*buff_ptr = HEX_DIGITS[value % base];
		
		chars_written ++;
		value /= base;
	}

	// If more characters still needed, use padding
	for(u32 i=chars_written; i<details.min_field_width; ++i)
		printer(details.is_zero_padded ? '0' : ' ');

	// Print out the actual buffer data
	while(chars_written--)
		printer(*buff_ptr++);
}

i32 printf(const char* format, ...) {
	
	// Function which actually receives characters
	i32 written = 0;
	const auto printer = [&written] (const char c) { written++; putc(c); };

	// Parse details and state machine
	enum State { CHAR=0, FORMING=10} state = CHAR;
	printf_details details{0,0,0};

	const auto end_field = [&]{ 
		details = printf_details{0,0,0};
		state = CHAR;
	};

	// %04d
	// %

	va_list parameters;
	va_start(parameters, format);
	for(;*format != '\0'; format++) {

		if(state == CHAR) {
			if(*format == '%') {
				state = FORMING;
				if(format[1] == '0') {
					format++;
					details.is_zero_padded = true;
				}
			}
			else
				printer(*format);
		}

		if(state == FORMING) {
			if(*format == 'l') {
				details.is_long = true;
			}
			else if(*format >= '0' && *format <= '9') {
				details.min_field_width = details.min_field_width*10 + (*format - '0');
			}

			else if(*format == 'x' && details.is_long) {
				u64 val = va_arg(parameters, u64);
				printf_dux(val, 16, details, printer);
				end_field();
			}
			else if(*format == 'x' && !details.is_long) {
				u32 val = va_arg(parameters, u32);
				printf_dux(val, 16, details, printer);
				end_field();
			}

			else if(*format == 'd' && details.is_long) {
				i64 val = va_arg(parameters, i64);
				printf_dux(val, 10, details, printer);
				end_field();
			}
			else if(*format == 'd' && !details.is_long) {
				i32 val = va_arg(parameters, i32);
				printf_dux(val, 10, details, printer);
				end_field();
			}

			// TODO: %s, %c

		}

	}
	va_end(parameters);

	return written;
}
 