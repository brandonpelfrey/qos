import os, sys, subprocess

########################################
# Root compilation settings, shared among all builds

env = Environment(
  AS='./toolchain/gcc-install/bin/i686-elf-as',
  CC='./toolchain/gcc-install/bin/i686-elf-gcc',
  CXX='./toolchain/gcc-install/bin/i686-elf-g++'
)
env['ENV']['TERM'] = os.environ['TERM'] # Color terminal
env.VariantDir('build/Kernel', 'Kernel', duplicate=0)

env.Append(CXXFLAGS = ['-std=c++17', '-ffreestanding', '-g', '-O0', '-Wall', '-Wextra', '-fno-exceptions', '-fno-rtti'])

# Kernel CPP files
kernel_objects = []
for root, dirs, files in os.walk("Kernel"):
  for file in files:
    if file.endswith(".cpp"):
      src_path = os.path.join(root, file)
      build_path = src_path.replace('Kernel/', 'build/Kernel/', 1)
      print(src_path)
      kernel_objects.append( env.Object(target=build_path, CCFLAGS=' -O0 -nostdlib -lgcc' ) )

# Bootloader
bootloader = env.Object('build/Kernel/boot.s')

# Kernel Image
kernel_env = env.Clone()
kernel_env['LINKFLAGS'] = '-T linker.ld -ffreestanding -O0 -nostdlib'
kernel_env.Program('build/qos_image.bin', source=[bootloader] + kernel_objects)
