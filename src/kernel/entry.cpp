
#include <hardware/drivers/bochs_graphics.h>
#include <hardware/pci/pci.h>
#include <hardware/ps2_8042/ps2_8042.h>
#include <interrupts/idt.h>
#include <interrupts/interrupts.h>
#include <interrupts/pic_8259.h>
#include <kernel_layout.h>
#include <libc.h>
#include <logging.h>
#include <memory/memory_map.h>
#include <terminal.h>
#include <utils/utils.h>
#include <utils/x86_64.h>

static klog_spec LOG{.module_name = "Entry", .text_color = Terminal::VGA_COLOR_LIGHT_BLUE};

void test_breakpoint_software_interrupt() { asm volatile("int 3"); }

#include "./font.h"

void write_glyph(const char c, u8* framebuffer) {
  static int gx = 0, gy = 0;
  u8* glyph = font8x8_basic[c];

  for (int row = 0; row < 8; ++row) {
    char row_data = glyph[row];
    for (int col = 0; col < 8; ++col) {
      framebuffer[640 * 8 * gy + 640 * row + 8 * gx + col] = ((row_data >> col) & 1) * 2;
    }
  }

  gx++;
  if (gx == 40) {
    gx = 0;
    gy++;
  }
}

void write_text(u8* framebuffer) {
  static const char* text = "Hello there!";
  for (const char* c = text; *c; ++c) write_glyph(*c, framebuffer);
}

extern "C" int __kernel_main(SMAP_entry* smap_data) {
  Terminal::initialize();
  kprintf(LOG, "Entered __kernel_main, Memory map @ 0x%p\n", smap_data);

  IDT::Initialize();
  test_breakpoint_software_interrupt();

  PIC::init();
  Hardware::PS2_8042::init();
  Interrupts::enable_hardware_interrupts();

  Hardware::PCI::init();
  Hardware::PCI::controller.enumerate_bus();

  //Hardware::Drivers::bga_driver.set_resolution(640, 480, 8);
  //u8* framebuffer = (u8*)Hardware::Drivers::bga_driver.get_framebuffer_start();
  //framebuffer -= 8;
  //write_text(framebuffer);

  while (1)
    ;
  kpanic("Reached end of __kernel_main. This should never happen.");
  return 0;
}
