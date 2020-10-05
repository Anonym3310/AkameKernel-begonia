#!/bin/bash


echo
echo "Build The Kernel"
echo 

time sudo make -j16 O=out CC=clang HOSTCC=clang CLANG_TRIPLE=aarch64-linux-gnu- CROSS_COMPILE=aarch64-linux-gnu- CROSS_COMPILE_ARM32=arm-linux-gnueabi- ARCH=arm64 SUBARCH=arm64

echo ""

echo "Build Anykernel"

echo ""