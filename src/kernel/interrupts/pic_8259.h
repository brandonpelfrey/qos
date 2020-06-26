#pragma once
#include <hardware/io.h>

namespace PIC
{
struct PIC_8259
{
  IO::Port8 command_port;
  IO::Port8 data_port;
};

void init();

extern PIC_8259 primary;
extern PIC_8259 secondary;

}; // namespace PIC
