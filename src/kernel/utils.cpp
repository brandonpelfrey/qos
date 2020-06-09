#include <terminal.h>
#include <utils.h>

void panic(const char* msg) {
  terminal::set_color_bg(terminal::VGA_COLOR_RED);
  terminal::set_color_fg(terminal::VGA_COLOR_WHITE);
  terminal::write("PANIC: ");
  terminal::write(msg);
  while (1) {
    asm("cli");
    asm("hlt");
  }
}