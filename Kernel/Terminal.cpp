#include "types.h"
#include "libc.h"

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};
static const unsigned VGA_WIDTH = 80;
static const unsigned VGA_HEIGHT = 25;

///////////////////////////////

unsigned terminal_row;
unsigned terminal_column;
u8 terminal_color;
u16* terminal_buffer;

///////////////////////////////

namespace terminal {

constexpr u8 vga_entry_color(enum vga_color fg, enum vga_color bg) 
{	return fg | bg << 4; }
 
constexpr u16 vga_entry(unsigned char uc, u8 color) 
{	return (u16) uc | (u16) color << 8; }

void initialize() 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (u16*) 0xB8000;
	for (unsigned y = 0; y < VGA_HEIGHT; y++) {
		for (unsigned x = 0; x < VGA_WIDTH; x++) {
			const unsigned index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void SetColor(u8 color) 
{	terminal_color = color; }
 
void putc(char c, u8 color, unsigned x, unsigned y) 
{
	const unsigned index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void scroll() {
	for(u32 row=0; row<VGA_HEIGHT-1; ++row) {
		u16 *src = &terminal_buffer[VGA_WIDTH * (row+1)];
		u16 *dst = src - VGA_WIDTH;
		memcpy(dst, src, VGA_WIDTH * sizeof(terminal_buffer[0]));
	}

	memset(&terminal_buffer[VGA_WIDTH * (VGA_HEIGHT-1)], 0, sizeof(terminal_buffer[0]) * VGA_WIDTH );
}

void putc(char c) 
{
	if(c == '\r') {
		terminal_column = 0;
		return;
	} else if (c == '\n') {
		terminal_column = VGA_WIDTH;
	} else {
		putc(c, terminal_color, terminal_column, terminal_row);
		terminal_column++;
	}

	if (terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT) {
			scroll();
			terminal_row = VGA_HEIGHT-1;
		}
	}
}

void write(const char* data, unsigned size) 
{
	for (unsigned i = 0; i < size; i++)
		putc(data[i]);
}
 
void write(const char* data) 
{
  for (auto ptr = data; *ptr != 0; ptr++)
		putc(*ptr);
}

};
