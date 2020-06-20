
#include <idt.h>
#include <kernel_layout.h>
#include <libc.h>
#include <logging.h>
#include <terminal.h>
#include <utils/utils.h>
#include <virtual_memory.h>

static klog_spec ENTRY{.module_name = "Entry", .text_color = terminal::VGA_COLOR_LIGHT_BLUE};

void test_breakpoint_software_interrupt() { asm volatile("int 3"); }

extern "C" int __kernel_main(vm::SMAP_entry* smap_data) {
  terminal::initialize();

  u64 q = get_rip();
  kprintf(ENTRY, "Kernel executing at Virtual address 0x%016lx\n", q);

  IDT::Initialize();
  test_breakpoint_software_interrupt();

  kpanic("Reached end of __kernel_main. This should never happen.");
  return 0;
}
