#!/bin/bash

echo " "

echo "Continued assembly"

echo " "


time sudo make -j16 O=out CC=clang HOSTCC=clang CLANG_TRIPLE=aarch64-linux-gnu- CROSS_COMPILE=aarch64-linux-gnu- CROSS_COMPILE_ARM32=arm-linux-gnueabi- ARCH=arm64 SUBARCH=arm64 PATH="/root/Clang-9.0.4/bin:${PATH}" LD_LIBRARY_PATH="/root/Clang-9.0.4/lib:/root/Clang-9.0.4/lib64:$LD_LIBRARY_PATH"

echo ""

echo "Build Anykernel"

echo ""

time cp `find -name *.ko` /root/AnyKernel3/modules/system/lib/modules

time cp `find -name Image.gz-dtb` /root/AnyKernel3

time cd /root/AnyKernel3 && zip -r -9 AkameKernel.zip *
