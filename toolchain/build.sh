#!/bin/bash

export SCRIPT_DIR=$(realpath $(dirname $0))

export PREFIX=$SCRIPT_DIR/gcc-install
export TARGET=i686-elf
export PATH=$PREFIX/bin:$PATH

export BINUTILS_VERSION="binutils-2.34"
export GCC_VERSION="gcc-10.1.0"

if [ ! -d $SCRIPT_DIR/gcc-source ]; then
  mkdir -p $SCRIPT_DIR/gcc-source
fi

if [ ! -e $SCRIPT_DIR/gcc-source/$BINUTILS_VERSION.tar.gz ]; then
  echo "Downloading $BINUTILS_VERSION..."
  pushd $SCRIPT_DIR/gcc-source
    curl -O "https://ftp.gnu.org/gnu/binutils/$BINUTILS_VERSION.tar.gz"
  popd
fi

if [ ! -e $SCRIPT_DIR/gcc-source/$GCC_VERSION.tar.gz ]; then
  echo "Downloading $GCC_VERSION..."
  pushd $SCRIPT_DIR/gcc-source
    curl -O "https://ftp.gnu.org/gnu/gcc/gcc-10.1.0/$GCC_VERSION.tar.gz"
  popd
fi

echo "Toolchain will install to: $PREFIX"

# Verify hashes of gcc source
pushd gcc-source
  echo "Extracting Binutils + GCC..."
  tar -xf $BINUTILS_VERSION.tar.gz
  tar -xf $GCC_VERSION.tar.gz

  echo "Building Binutils..."
  mkdir -p build-binutils
  pushd build-binutils
    ../$BINUTILS_VERSION/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
    make -j
    make install
  popd

  which -- $TARGET-as || echo "$TARGET-as is not in the PATH"

  mkdir build-gcc;
  pushd build-gcc
    ../$GCC_VERSION/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
    make -j all-gcc
    make -j all-target-libgcc
    make install-gcc
    make install-target-libgcc
  popd

popd

