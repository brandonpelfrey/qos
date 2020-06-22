#include <types.h>
#include <utils/x86_64.h>

namespace System::IO {
void outb(u16 port, u8 val) { asm volatile("out %0, %1" : : "d"(port), "a"(val)); }
void outw(u16 port, u16 val) { asm volatile("out %0, %1" : : "d"(port), "a"(val)); }
void outl(u16 port, u32 val) { asm volatile("out %0, %1" : : "d"(port), "a"(val)); }

u8 inb(u16 port) {
  u8 ret;
  asm volatile("in %0, %1" : "=a"(ret) : "d"(port));
  return ret;
}
u16 inw(u16 port) {
  u8 ret;
  asm volatile("in %0, %1" : "=a"(ret) : "d"(port));
  return ret;
}
u32 inl(u16 port) {
  u8 ret;
  asm volatile("in %0, %1" : "=a"(ret) : "d"(port));
  return ret;
}
};  // namespace System::IO
