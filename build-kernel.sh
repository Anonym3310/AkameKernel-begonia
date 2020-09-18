#!/bin/bash

echo
echo "Clean Build Directory"
echo 

sudo make clean proper CC=clang HOSTCC=clang -j16
rm -rf out
echo
echo "Create Working Directory"
echo

mkdir -p out

echo
echo "Set DEFCONFIG"
echo 
time sudo make O=out CC=clang HOSTCC=clang akame_defconfig ARCH=arm64 SUBARCH=arm64 -j16
export PATH="/root//bin:${PATH}"
export LD_LIBRARY_PATH="/root//lib:/root//lib64:$LD_LIBRARY_PATH"

echo
echo "Build The Kernel"
echo 

time sudo make -j9 O=out CC=clang HOSTCC=clang CLANG_TRIPLE=aarch64-linux-gnu- CROSS_COMPILE=aarch64-linux-gnu- CROSS_COMPILE_ARM32=arm-linux-gnueabi- ARCH=arm64 SUBARCH=arm64

echo ""

echo "Build Anykernel"

echo ""

time cp `find -name *.ko` /sdcard/Download/Test./modules/system/lib/modules

time cp `find -name Image.gz-dtb` /sdcard/Download/Test.

time cd /sdcard/Download/Test. && zip -r -9 AkameKernel.zip *
