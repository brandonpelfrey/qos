#pragma once

#include "types.h"

namespace terminal {
void initialize();
void putc(char c);
void putc(char c, u8 color, unsigned x, unsigned y);

void write(const char *data);
void write(const char *data, unsigned size);

}  // namespace terminal
