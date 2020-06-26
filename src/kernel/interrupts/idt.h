#include <types.h>

namespace IDT
{

namespace Interrupts
{
enum
{
  DIVIDE_BY_ZERO = 0,
  DEUBG,
  NMI,
  BREAKPOINT,
  OVERFLOW,
  BOUND_RANGE_EXCEEDED,
  INVALID_OPCODE,
  DEVICE_NOT_AVAILABLE,
  DOUBLE_FAULT,
  COPROCESSOR_SEGMENT_OVERRUN,
  INVALID_TSS,
  SEGMENT_NOT_PRESENT,
  STACK_SEGMENT_FAULT,
  GENERAL_PROTECTION_FAULT,
  PAGE_FAULT,
  RESERVED_0,
  FLOATING_POINT_EXCEPTION,
  ALIGNMENT_CHECK,
  MACHINE_CHECK,
  SIMD_FLOATING_POINT_EXCEPTION,
  VIRTUALIZATION_EXCEPTION,
  RESERVED_1,
  SECURITY_EXCEPTION,
  RESERVED_2,
  TRIPLE_FAULT,
  N_INTERRUPTS
};
}

struct Entry
{
  void (*handler)();
  struct
  {
    u8 interrupt_stack_index;
    u8 is_trap_gate;
    u8 descriptor_priviledge_level;
    bool present;
  } options;
};

void Initialize();
void Register(Entry& descriptor);

} // namespace IDT