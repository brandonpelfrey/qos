#include <libc.h>
#include <stdarg.h>
#include <terminal.h>
#include <utils/utils.h>
#include <utils/x86_64.h>

[[noreturn]] void kpanic(const char* msg, ...)
{
  Terminal::set_color_bg(Terminal::VGA_COLOR_RED);
  Terminal::set_color_fg(Terminal::VGA_COLOR_WHITE);
  
  printf("Kernel Panic\n");

  va_list parameters;
  va_start(parameters, msg);
  printf_valist(msg, parameters);
  va_end(parameters);

  // Dump registers
  System::RegisterStates reg_states;
  System::dump_register_states(&reg_states);

  printf("RAX 0x%016lx\n", reg_states.rax);
  printf("RBX 0x%016lx\n", reg_states.rbx);
  printf("RCX 0x%016lx\n", reg_states.rcx);
  printf("RDX 0x%016lx\n", reg_states.rdx);
  printf("RSP 0x%016lx\n", reg_states.rsp);

  while (1)
  {
    asm("cli");
    asm("hlt");
  }
}
