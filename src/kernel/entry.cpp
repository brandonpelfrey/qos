
#include <interrupts/idt.h>
#include <interrupts/interrupts.h>
#include <interrupts/pic_8259.h>
#include <hardware/ps2_8042/ps2_8042.h>

#include <kernel_layout.h>
#include <libc.h>
#include <logging.h>
#include <terminal.h>
#include <utils/utils.h>
#include <utils/x86_64.h>
#include <virtual_memory.h>

static klog_spec LOG{.module_name = "Entry", .text_color = Terminal::VGA_COLOR_LIGHT_BLUE};

void test_breakpoint_software_interrupt() { asm volatile("int 3"); }

extern "C" int __kernel_main(vm::SMAP_entry* smap_data) {
  Terminal::initialize();
  kprintf(LOG, "Entered __kernel_main, Memory map @ 0x%p\n", smap_data);

  IDT::Initialize();
  test_breakpoint_software_interrupt();

  PIC::init();
  Hardware::PS2_8042::init();
  Interrupts::enable_hardware_interrupts();
  
  while(1);
  kpanic("Reached end of __kernel_main. This should never happen.");
  return 0;
}
