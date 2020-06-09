#include <logging.h>
#include <libc.h>
#include <stdarg.h>

int kprintf(klog_spec& spec, const char* format, ...) {
  // TODO : save original color
  terminal::set_color_fg(spec.text_color);
  printf("[%s] ", spec.module_name);
  terminal::set_color_fg(terminal::VGA_COLOR_LIGHT_GREY);

  va_list parameters;
  va_start(parameters, format);
  const auto written = printf_valist(format, parameters);
  va_end(parameters);
  return written;
}