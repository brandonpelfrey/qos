
#include <idt.h>
#include <kernel_layout.h>
#include <libc.h>
#include <types.h>
#include <utils/utils.h>

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

struct interrupt_frame;
__attribute__((interrupt)) void breakpoint_test(struct interrupt_frame* frame) {
  printf("handling breakpoint interrupt\n");
}

void IDT::Initialize() {
  memset(__IDT, 0, sizeof(__IDT));

  void (*fptr)(struct interrupt_frame*) = &breakpoint_test;

  __IDT[Interrupts::BREAKPOINT] = {.function_0_15 = ((u64)fptr) & 0xFFFF,
                                   .gdt_selector = 0x08,
                                   .always_zero = 0,
                                   .flags = 0b10001110,
                                   .function_16_31 = (((u64)fptr) >> 16) & 0xFFFF,
                                   .function_32_63 = (((u64)fptr) >> 32) & 0xFFFFFFFF};

  // Load this as the current IDT
  __IDTPTR.base = (u64)&__IDT[0];
  __IDTPTR.limit = sizeof(__IDT) - 1;

  printf("IDT : RIP   @ 0x%016lx\n", get_rip());
  printf("IDT : Table @ 0x%016lx\n", __IDTPTR.base);

  asm volatile("lidtq %0" : : "m"(__IDTPTR) : "memory");
}
