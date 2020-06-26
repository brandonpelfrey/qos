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
# Kernel files

env['CXXFLAGS'] = '-std=c++17 -ffreestanding -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti -masm=intel -Isrc/kernel -mno-sse -mno-mmx -mno-80387 -mno-red-zone -mcmodel=kernel'

# Walk through and find every .cpp/.s file, give them a unique name based on path and file type
# The naming scheme here is designed to play nice with the linker script's ability to find files.
kernel_objects = []
for root, dirs, files in os.walk("src/kernel"):
  for file in files:
    if file.endswith(".cpp") or file.endswith(".s"):
      src_path = os.path.join(root, file)
      build_path = src_path.replace('src/kernel/', 'build/kernel/', 1)

      # Replace the full path inside kernel/... with _ for simple linking in the linker script
      build_path_part = 'build/kernel'
      build_prefix, build_suffix = build_path.split(build_path_part)
      build_suffix = build_suffix.replace('/', '_').replace('.cpp', '_cpp.o').replace('.s', '_s.o')
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
bootloader_linked = env.Command('build/bootloader-linked.o', [boot1, boot2, 'src/linker-boot.ld'], "$LD -T src/linker-boot.ld -o $TARGET")
kernel_linked = env.Command('build/kernel-linked.o', kernel_objects + ['src/linker-kernel.ld'], "$LD -T  src/linker-kernel.ld -o $TARGET build/kernel_*.o")

bootloader_image = env.Command('build/bootloader-image.bin', bootloader_linked, "$OBJCOPY -O binary $SOURCE $TARGET")
kernel_image = env.Command('build/kernel-image.bin', kernel_linked, "$OBJCOPY -O binary $SOURCE $TARGET")

# Cat together
qos_image = env.Command('build/image.bin', [bootloader_image, kernel_image], "cat $SOURCES > $TARGET")
