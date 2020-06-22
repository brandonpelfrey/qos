# QOS
![Latest screenshot](https://user-images.githubusercontent.com/407441/85248914-1b0f9c00-b407-11ea-88ad-d8f69b080d4b.png)

# Summary
TODO

# Prerequisites to build (MacOS X)
Install Homebrew, then...
```
# Install scons as the build tool
brew install scons

# Install NASM, GCC, and binutils for actually building the kernel
brew install nasm
brew install x86_64-elf-gcc 
brew install x86_64-elf-binutils

# Optional: Install emulators and gdb for testing
brew install qemu bochs gdb
```

# Build and run
```
scons && qemu-system-x86_64 -fda build/image.bin -m 512
```

