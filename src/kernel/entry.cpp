
#include <terminal.h>
#include <virtual_memory.h>
#include <libc.h>
#include <utils.h>
#include <logging.h>

static klog_spec ENTRY { .module_name="Entry", .text_color=terminal::VGA_COLOR_LIGHT_BLUE };

size_t get_rip()
{
  register u64 a asm("rax");
  asm("lea rax, [rip+0]");
  return a;
}

void entry_virtual_addressing() {
  u64 q = get_rip();
  kprintf(ENTRY, "Kernel executing at Virtual address 0x%016lx\n", q);

  while(1);
}

extern "C"
int __kernel_main(vm::SMAP_entry* smap_data) {
  terminal::initialize();
  vm::initialize(smap_data);

  // Start running the rest of the kernel, but let's first jump into kernel in high address.
  u64 high_addr = 0xFFFFFFFF80000000;
  u64 step2_addr = high_addr + ((u64)entry_virtual_addressing);
  ((void(*)())step2_addr)();

  panic("Reached end of __kernel_main. This should never happen.");
  return 0;
}
