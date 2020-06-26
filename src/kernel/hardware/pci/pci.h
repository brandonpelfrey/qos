#pragma once

#include <hardware/io.h>

// Bus (Up to 8 per controller)
// Devices (32 Devices per bus)
// Function (Up to eight functions each device -- Audio, Graphics, etc.)

// vendor_id, device_id
// class_id, subclass_id

namespace Hardware::PCI
{

struct Controller
{
  IO::Port32 data_port;
  IO::Port32 command_port;

  u32 read(u16 bus, u16 device, u16 function, u32 register_offset);
  void write(u16 bus, u16 device, u16 function, u32 register_offset, u32 value);
  void enumerate_bus();
};

extern Controller controller;

void init();

}
