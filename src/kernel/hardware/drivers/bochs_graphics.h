#pragma once

#include <hardware/io.h>
#include <types.h>

namespace Hardware::Drivers
{
/* "BGA" (PCIVendor 1234, Device 1111)
https://wiki.osdev.org/Bochs_VBE_Extensions
*/
struct BochsGraphicsAdapter
{
private:
  void write_reg(u16 reg, u16 data);

  // Base address of linear framebuffer
  u64 lfb_address;

public:
  IO::Port16 io_port;
  IO::Port16 data_port;

  void init(u8 pci_bus, u8 device_num);
  void set_resolution(u16 xres, u16 yres, u8 bits_per_pixel);

  u64 get_framebuffer_start();
};

extern BochsGraphicsAdapter bga_driver;
} // namespace Hardware::Drivers