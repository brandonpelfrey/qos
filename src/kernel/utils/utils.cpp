#include <terminal.h>
#include <utils/utils.h>

struct x86_64_registers {
  u64 rax, rbx, rcx, rdx;
  u64 rsp, rbp, rsi, rdi;
  u64 r8, r9, r10, r11, r12, r13, r14, r15;
  u64 rip;
  u64 cr0;
  u64 cr2;
  u64 cr3;
  u64 cr4;
};

void kpanic(const char* msg) {
  terminal::set_color_bg(terminal::VGA_COLOR_RED);
  terminal::set_color_fg(terminal::VGA_COLOR_WHITE);
  terminal::write("PANIC: ");
  terminal::write(msg);

  // TODO : Dump registers

  while (1) {
    asm("cli");
    asm("hlt");
  }
}

size_t get_rip()
{
  register u64 a asm("rax");
  asm("lea rax, [rip+0]");
  return a;
}
