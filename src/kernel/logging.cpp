#include <libc.h>
#include <logging.h>
#include <stdarg.h>

int kprintf(klog_spec& spec, const char* format, ...)
{
  // TODO : save original color
  Terminal::set_color_fg(spec.text_color);
  printf("[%s] ", spec.module_name);
  Terminal::set_color_fg(Terminal::VGA_COLOR_LIGHT_GREY);

  va_list parameters;
  va_start(parameters, format);
  const auto written = printf_valist(format, parameters);
  va_end(parameters);
  return written;
}