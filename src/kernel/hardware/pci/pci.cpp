#include "pci.h"

#include <hardware/drivers/bochs_graphics.h>
#include <logging.h>

namespace Hardware::PCI
{

static klog_spec LOG{ .module_name = "PCI", .text_color = Terminal::VGA_COLOR_LIGHT_BLUE };

Controller controller;

struct PCIDeviceDescription
{
  u16 vendor_id;
  u16 device_id;
  const char* vendor_name;
  const char* device_name;
};

const PCIDeviceDescription DEVICE_REGISTRY_LIST[] = {
  { 0x8086, 0x1237, "Intel", "82441FX PCI/Memory Controller" },
  { 0x8086, 0x7000, "Intel", "82371SB PIIX3 ISA (Southbridge)" },
  { 0x8086, 0x100E, "Intel", "82540EM Gigabit Ethernet Controller" },
  { 0x1234, 0x1111, "QEMU", "Virtual Video Controller" },
};

const PCIDeviceDescription get_device_description(const u16 vendor_id, const u16 device_id)
{
  PCIDeviceDescription result{ vendor_id, device_id, "UnknownVendor", "UnknownDevice" };

  for (size_t i = 0; i < sizeof(DEVICE_REGISTRY_LIST) / sizeof(PCIDeviceDescription); ++i)
  {
    const auto& descriptor = DEVICE_REGISTRY_LIST[i];

    if (descriptor.vendor_id == vendor_id)
    {
      result.vendor_name = descriptor.vendor_name;
      if (descriptor.device_id == device_id)
      {
        result.device_name = descriptor.device_name;
        break;
      }
    }
  }

  return result;
}

void init()
{
  controller = { .data_port = 0xCFC, .command_port = 0xCF8 };
}

inline u32 create_id(u16 bus, u16 device, u16 function, u32 register_offset)
{
  u32 id = 1 << 31;
  id |= (bus & 0xFF) << 16;
  id |= (device & 0x1F) << 11;
  id |= (function & 0x07) << 8;
  id |= (register_offset & 0xFC);
  return id;
}

u32 Controller::read(u16 bus, u16 device, u16 function, u32 register_offset)
{
  const u32 id = create_id(bus, device, function, register_offset);
  command_port.write(id);

  // You can only read a full 32 bits at a time, so return the correct offset in this double word
  const u32 data_dword = data_port.read();
  return data_dword >> (8 * (register_offset & 0b11));
}
void Controller::write(u16 bus, u16 device, u16 function, u32 register_offset, u32 value)
{
  const u32 id = create_id(bus, device, function, register_offset);
  command_port.write(id);
  data_port.write(value);
}

void Controller::enumerate_bus()
{
  kprintf(LOG, "Enumerating PCI Bus devices..\n");

  for (int bus = 0; bus < 256; ++bus)
    for (int device = 0; device < 32; ++device)
    {
      const u16 vendor_id = read(bus, device, 0, 0);
      if (vendor_id != 0xFFFF)
      {
        const u16 device_id = read(bus, device, 0, 2);

        const auto descriptor = get_device_description(vendor_id, device_id);
        kprintf(
          LOG, " - (0x%04x:0x%04x) %s %s\n", vendor_id, device_id, descriptor.vendor_name, descriptor.device_name);

        if (vendor_id == 0x1234 && device_id == 0x1111)
        {
          // Hardware::Drivers::bga_driver.init(bus, device);
        }
      }
    }
}

} // namespace Hardware::PCI