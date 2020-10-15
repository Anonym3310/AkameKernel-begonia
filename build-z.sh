#!/bin/bash

echo  " "

echo "Build kernel-headers"

echo  " "

sudo rm -rf ../headers-begonia/

sudo rm -rf ../tmp 

mkdir ../headers-begonia

cp -r * ../headers-begonia

cd ../headers-begonia

time sudo make akame4.4_defconfig CC=clang-9 CROSS_COMPILE=aarch64-linux-gnu- CLANG_TRIPLE=aarch64-linux-gnu-  CROSS_COMPILE_ARM32=arm-linux-gnueabi- -j9
    
time sudo make prepare CC=clang-9 CROSS_COMPILE=aarch64-linux-gnu- CLANG_TRIPLE=aarch64-linux-gnu- CROSS_COMPILE_ARM32=arm-linux-gnueabi- -j9

mkdir ../tmp && cp -r arch/arm* Makefile scripts/mod/modpost include scripts drivers/misc ../tmp && rm -rf * && cp -r ../tmp/* $PWD && mv modpost scripts/mod && rm -rf ../tmp && mkdir arch && cp -r arm* arch  && rm -rf arm* && mkdir drivers && cp -r misc drivers && rm -rf misc

cd ../headers-begonia && tar czf headers-begonia.tar.xz *
ls

