#pragma once
#include <types.h>

namespace IO
{

template<typename port_data_type>
struct Port
{
  u16 port_number;

  Port()
  {
  }
  Port(u16 port_number)
    : port_number(port_number)
  {
  }
  port_data_type read()
  {
    port_data_type ret;
    asm volatile("in %0, %1" : "=a"(ret) : "d"(port_number));
    return ret;
  }

  Port<port_data_type>& write(port_data_type data)
  {
    asm volatile("out %0, %1" : : "d"(port_number), "a"(data));
    return *this;
  }
  Port<port_data_type>& wait()
  {
    asm volatile("out 0x80, %%al" : : "Na"(0));
    return *this;
  }
};

using Port8 = Port<u8>;
using Port16 = Port<u16>;
using Port32 = Port<u32>;
} // namespace IO
