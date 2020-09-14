#!/bin/bash

echo
echo "Clean Build Directory"
echo 

sudo make clean && make mrproper CC=clang HOSTCC=clang
rm -rf out
echo
echo "Create Working Directory"
echo

mkdir -p out

echo
echo "Set DEFCONFIG"
echo 
time sudo make O=out CC=clang HOSTCC=clang genom_defconfig ARCH=arm64 SUBARCH=arm64
export PATH="/root//bin:${PATH}"
export LD_LIBRARY_PATH="/root//lib:/root//lib64:$LD_LIBRARY_PATH"

echo
echo "Build The Kernel"
echo 

time sudo make -j9 O=out CC=clang HOSTCC=clang CLANG_TRIPLE=aarch64-linux-gnu- CROSS_COMPILE=aarch64-linux-gnu- CROSS_COMPILE_ARM32=arm-linux-gnueabi- ARCH=arm64 SUBARCH=arm64
