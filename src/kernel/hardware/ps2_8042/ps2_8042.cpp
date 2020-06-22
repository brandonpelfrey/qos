#include "ps2_8042.h"

#include <libc.h>
#include <logging.h>
#include <types.h>
static klog_spec LOG{.module_name = "PS2", .text_color = Terminal::VGA_COLOR_LIGHT_GREEN};

namespace Hardware::PS2_8042 {
PS2_Controller_8042 keyboard;

#define STATUS_OUTPUT_BUFFER_IS_FULL_BIT 0x1

#define COMMAND_READ_CONTROLLER_BYTE_0 0x20
#define COMMAND_WRITE_CONTROLLER_BYTE_0 0x20
#define COMMAND_DISABLE_FIRST_PORT 0xAD
#define COMMAND_ENABLE_FIRST_PORT 0xAE

static const u8 US_KEYBOARD_SCAN_CODES[128] = {
  '.', '.', '1', '2', '3', '4', '5', '6', // 0x00
  '7', '8', '9', '0', '-', '=', '.', '\t', // 0x08
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', // 0x10
  'O', 'P', '[', ']', '\n', '.', 'A', 'S', // 0x18
  'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', // 0x20
  '\'', '`', '.', '.', 'Z', 'X', 'C', 'V', // 0x28
  'B', 'N', 'M', ',', '.', '/', '.', '.', // 0x30
  '.', ' ', '.', '.', '.', '.', '.', '.', // 0x38
  '.', '.', '.', '.', '.', '.', '.', '.', // 0x40
  '.', '.', '.', '.', '.', '.', '.', '.', // 0x48
  '.', '.', '.', '.', '.', '.', '.', '.', // 0x50
  '.', '.', '.', '.', '.', '.', '.', '.', // 0x58
};

void init() {
  keyboard = {.command_port = 0x64, .data_port = 0x60};

  // While key is still being held at startup, sit here and wait for it to be released.
  while (keyboard.command_port.read() & STATUS_OUTPUT_BUFFER_IS_FULL_BIT) {
    keyboard.data_port.read();
  }

  // Enable PS2 first port
  keyboard.command_port.write(COMMAND_ENABLE_FIRST_PORT);

  // Configure the controller a little
  keyboard.command_port.write(COMMAND_READ_CONTROLLER_BYTE_0);

  u8 status = keyboard.data_port.read();
  status |= 1;      // Enable interrupts for the first port
  status &= ~0x10;  // Enable clock for the first port

  keyboard.command_port.write(COMMAND_WRITE_CONTROLLER_BYTE_0);
  keyboard.data_port.write(status);

  kprintf(LOG, "Initialized PS/2 Keyboard.\n");
}

void PS2_Controller_8042::handle_irq() {
  const u8 data = data_port.read();

  if (data >= 0xFA || data == 0x00)  // These are status/internal failure stuff
    return;

  const u8 scan_code = data & 0x7F;
  const u8 ascii = US_KEYBOARD_SCAN_CODES[scan_code];

  if((data & 0x80) == 0)
    printf("%c", ascii);

  //kprintf(LOG, "Handling IRQ : 0x%02x = '%c'\n", scan_code, ascii);

  // TODO : Needs a piping mechanism to dispatch events to those interested..
}

}  // namespace Hardware::PS2_8042
