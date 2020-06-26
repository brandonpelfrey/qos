#include "interrupts.h"

namespace Interrupts
{
void enable_hardware_interrupts()
{
  asm volatile("sti");
}
void disable_hardware_interrupts()
{
  asm volatile("cli");
}

} // namespace Interrupts