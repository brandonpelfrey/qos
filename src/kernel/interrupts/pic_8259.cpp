#include "pic_8259.h"

namespace PIC
{

PIC_8259 primary;
PIC_8259 secondary;

void init()
{
  ///////////////////////////////
  // Setup the primary PIC
  primary = { .command_port = 0x20, .data_port = 0x21 };

  // Start initialization procedure for primary PIC
  primary.command_port.write(0x11);

  primary.data_port.write(0x20).wait(); // Interrupt offset begings at 0x20
  primary.data_port.write(0x04).wait(); // There's a second PIC at IRQ2 (the secondary)
  primary.data_port.write(0x01).wait(); // We're in 8086 Mode

  ///////////////////////////////
  // Setup the secondary PIC
  secondary = { .command_port = 0xA0, .data_port = 0xA1 };

  // Start initialization procedure for secondary PIC
  secondary.command_port.write(0x11);

  secondary.data_port.write(0x28).wait(); // Interrupt offset begings at 0x28
  secondary.data_port.write(0x02).wait(); // No more cascading from this secondary PIC
  secondary.data_port.write(0x01).wait(); // We're in 8086 Mode
}

} // namespace PIC