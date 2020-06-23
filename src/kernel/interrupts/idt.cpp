
#include "idt.h"

#include <interrupts/pic_8259.h>
#include <hardware/ps2_8042/ps2_8042.h>
#include <kernel_layout.h>
#include <libc.h>
#include <logging.h>
#include <types.h>
#include <utils/utils.h>
#include <utils/x86_64.h>

#include "interrupts.h"

#define NUM_IRQS 64

extern u64 isr_table[];

static klog_spec LOG{.module_name = "Int", .text_color = Terminal::VGA_COLOR_LIGHT_BLUE};

struct IDTEntry {
  u16 function_0_15;
  u16 gdt_selector;
  u8 always_zero;
  u8 flags;
  u16 function_16_31;
  u32 function_32_63;
  u32 _reserved;
} __attribute__((packed)) __attribute__((aligned(16)));

struct IDTPointer {
  u16 limit;
  u64 base;
} __attribute__((packed));

IDTEntry __IDT[256] __attribute__((aligned(4096)));
IDTPointer __IDTPTR;

void InstallHandler(int irq_num, void* isr_handler_address) {
  __IDT[irq_num] = {.function_0_15 = (u16)(((u64)isr_handler_address) & 0xFFFF),
                    .gdt_selector = 0x08,
                    .always_zero = 0,
                    .flags = 0b10001110,
                    .function_16_31 = (u16)((((u64)isr_handler_address) >> 16) & 0xFFFF),
                    .function_32_63 = (u32)((((u64)isr_handler_address) >> 32) & 0xFFFFFFFF)};
}

extern "C" void global_interrupt_handler(u64 irq_number, void* rsp) {

  switch (irq_number) {
    
    case 0x03:
      kprintf(LOG, "IRQ(Software Breakpoint,%d) -- RSP @ %p\n", irq_number, rsp);
      break;

    case 0x20:
      //kprintf(LOG, "IRQ(Timer,%d) -- RSP @ %p\n", irq_number, rsp);
      break;
    
    case 0x21:
      //kprintf(LOG, "IRQ(Keyboard,%d) -- RSP @ %p\n", irq_number, rsp);
      Hardware::PS2_8042::keyboard.handle_irq();
      break;

    default:
      kprintf(LOG, "Unhandled interrupt 0x%0x\n", irq_number);
      kpanic("Unhandled interrupt");
  }

  // PIC Acknowledge
  if (irq_number >= 0x20 && irq_number < 0x30) {
    PIC::primary.command_port.write(0x20);
    if (irq_number >= 0x28) PIC::secondary.command_port.write(0x20);
  }
}

void IDT::Initialize() {
  memset(__IDT, 0, sizeof(__IDT));

  for (int i = 0; i < NUM_IRQS; ++i) {
    InstallHandler(i, (void*)isr_table[i]);
  }

  // Load this as the current IDT
  __IDTPTR.base = (u64)&__IDT[0];
  __IDTPTR.limit = sizeof(__IDT) - 1;

  asm volatile("lidtq %0" : : "m"(__IDTPTR) : "memory");
  kprintf(LOG, "Initialized IDT @ 0x%p\n", __IDTPTR.base);
}
