#pragma once
#include <types.h>

namespace Hardware::PCI {
  struct Vendor { 
    const char *vendor_name;
    const u16 vendor_id;
  };
};