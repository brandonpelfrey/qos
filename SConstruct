import os

########################################
# Root compilation settings, shared among all builds

env = Environment(
  AS='nasm',
  CC='x86_64-elf-gcc',
  CXX='x86_64-elf-gcc',
  LD='x86_64-elf-ld',
  OBJCOPY='x86_64-elf-objcopy',
)

env['ENV']['TERM'] = os.environ['TERM'] # Color terminal

################################################
# Kernel CPP files
#env.VariantDir('build/kernel', 'src/kernel', duplicate=0)
env['CXXFLAGS'] = '-std=c++17 -ffreestanding -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti -masm=intel -Isrc/kernel'

kernel_objects = []
for root, dirs, files in os.walk("src/kernel"):
  for file in files:
    if file.endswith(".cpp"):
      src_path = os.path.join(root, file)
      build_path = src_path.replace('src/kernel/', 'build/kernel/', 1)

      # Replace the full path inside kernel/... with _ for simple linking in the linker script
      build_path_part = 'build/kernel'
      build_prefix, build_suffix = build_path.split(build_path_part)
      build_suffix = build_suffix.replace('/', '_').replace('.cpp', '.o')
      build_path = build_prefix + build_path_part + build_suffix

      kernel_objects.append( env.Object(target=build_path, source=src_path) )

################################################
# Bootloader
env.VariantDir('build/bootloader', 'src/bootloader', duplicate=0)
env['ASFLAGS'] = '-f elf64'

boot1 = env.Object('build/bootloader/boot1.s')
boot2 = env.Object('build/bootloader/boot2.s')

################################################
# Linking 
qos_linked_elf64 = env.Command('build/qos_linked.o', [boot1, boot2] + kernel_objects, "$LD -T src/linker.ld -o $TARGET build/kernel_*.o")
qos_image = env.Command('build/image.bin', qos_linked_elf64, "$OBJCOPY -O binary $SOURCE $TARGET")
