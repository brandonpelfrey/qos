
# Building QOS Toolchain
The `build.sh` script will download from GNU's FTP server binutils and gcc, then build both of them, installing to gcc-install folder, which is expected by the rest of the build system for QOS.

### Build Dependencies
If you are on Ubuntu, you should first install :
```
sudo apt install libgmp-dev libmpfr-dev libmpc-dev texinfo
```