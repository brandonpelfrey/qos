#include "bochs_graphics.h"

#include <hardware/pci/pci.h>
#include <logging.h>

#define VBE_DISPI_INDEX_ID (0)
#define VBE_DISPI_INDEX_XRES (1)
#define VBE_DISPI_INDEX_YRES (2)
#define VBE_DISPI_INDEX_BPP (3)
#define VBE_DISPI_INDEX_ENABLE (4)
#define VBE_DISPI_INDEX_BANK (5)
#define VBE_DISPI_INDEX_VIRT_WIDTH (6)
#define VBE_DISPI_INDEX_VIRT_HEIGHT (7)
#define VBE_DISPI_INDEX_X_OFFSET (8)
#define VBE_DISPI_INDEX_Y_OFFSET (9)

#define VBE_DISPI_LFB_ENABLED (0x40)

namespace Hardware::Drivers
{

static klog_spec LOG{ .module_name = "PCI", .text_color = Terminal::VGA_COLOR_LIGHT_BLUE };

BochsGraphicsAdapter bga_driver;

void BochsGraphicsAdapter::write_reg(u16 reg, u16 value)
{
  bga_driver.io_port.write(reg);
  bga_driver.data_port.write(value);
}

void BochsGraphicsAdapter::init(u8 pci_bus, u8 device_num)
{
  u32 BAR0 = Hardware::PCI::controller.read(pci_bus, device_num, 0, 0x10);
  BAR0 &= 0xFFFFFFF0 | (0b11111100);

  bga_driver.io_port = 0x01CE;
  bga_driver.data_port = 0x1CF;
  bga_driver.lfb_address = BAR0;

  kprintf(LOG, "Initialized Bochs Graphics Adapter\n");
  kprintf(LOG, "  - Framebuffer @ 0x%p\n", BAR0);
}

void BochsGraphicsAdapter::set_resolution(u16 xres, u16 yres, u8 bits_per_pixel)
{
  // Disable the display, set relevant registers, then re-enable
  write_reg(VBE_DISPI_INDEX_ENABLE, 0);
  write_reg(VBE_DISPI_INDEX_XRES, xres);
  write_reg(VBE_DISPI_INDEX_YRES, yres);
  write_reg(VBE_DISPI_INDEX_BPP, bits_per_pixel);
  write_reg(VBE_DISPI_INDEX_ENABLE, 1 | VBE_DISPI_LFB_ENABLED);
}

u64 BochsGraphicsAdapter::get_framebuffer_start()
{
  return lfb_address;
}

} // namespace Hardware::Drivers