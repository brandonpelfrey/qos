#pragma once
#include <hardware/io.h>

namespace Hardware::PS2_8042
{
struct PS2_Controller_8042
{
  IO::Port8 command_port;
  IO::Port8 data_port;

  void handle_irq();
};

void init();
extern PS2_Controller_8042 keyboard;

} // namespace Hardware::PS2_8042
