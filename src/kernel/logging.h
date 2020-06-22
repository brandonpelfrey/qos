#pragma once

#include <terminal.h>

struct klog_spec {
  const char* module_name;
  Terminal::vga_color text_color;
};

int kprintf(klog_spec& spec, const char* format, ...);

