#!/bin/bash

pwd=$PWD
DATE=$(date +'%d-%m-%Y')
BUILD_START=$(date +"%s")
blue='\033[0;34m'
cyan='\033[0;36m'
yellow='\033[0;33m'
red='\033[0;31m'
nocol='\033[0m'



echo -e "$red\n ##----------------------------------------------------------------------------##$nocol\n"
echo -e "$blue\n     #### ##     ## ##     ##  #######  ########  ########    ###    ##       "
echo -e "      ##  ###   ### ###   ### ##     ## ##     ##    ##      ## ##   ##       "
echo -e "      ##  #### #### #### #### ##     ## ##     ##    ##     ##   ##  ##       "
echo -e "      ##  ## ### ## ## ### ## ##     ## ########     ##    ##     ## ##       "
echo -e "      ##  ##     ## ##     ## ##     ## ##   ##      ##    ######### ##       "
echo -e "      ##  ##     ## ##     ## ##     ## ##    ##     ##    ##     ## ##       "
echo -e "     #### ##     ## ##     ##  #######  ##     ##    ##    ##     ## ########$nocol"
echo -e "$red\n ##----------------------------------------------------------------------------##$nocol\n"

#################
#==[ Export ]===#
#################

#===[ Most Editable ]===#

DEFCONFIG=akame_defconfig
NKD=AkameKernel-begonia
CODENAME=begonia
GCC_or_CLANG=1
BUILD_KH=2
ONLY_BUILD_KH=1
ONLY_BUILD_AN=1
IMAGE=Image.gz-dtb
VER="-11"
ANDROID=Q
COMPILE="$IMAGE modules_install dtb dtbo.img"

#===[ Editable ]===#

#export USE_CCACHE=1
#export CCACHE_DIR=~/.ccache
JOBS="-j14"

#===[ Standart ]===#

export HOST_ARCH=$(arch)
ANYKERNEL_DIR=AnyKernel3
OUT_DIR=out
ARCH=arm64
SUBARCH=$ARCH
UN=$HOME/kernels
CONFIG=".config"
LOG="2>&1 | tee log.txt"
KBUILD_BUILD_USER=Anonym3310
KBUILD_KVER="-AkameKernel"
KBUILD_BUILD_HOST=anonym3310
KERNEL_MAKE_ENV="DTC_EXT=/usr/bin/dtc CONFIG_BUILD_ARM64_DT_OVERLAY=y"
CLANG_VAL="OBJCOPY=llvm-objcopy${VER} \
          OBJDUMP=llvm-objdump${VER} \
          STRIP=llvm-strip${VER} \
          NM=llvm-nm${VER} \
          AR=llvm-ar${VER} \
          AS=llvm-as${VER}"
LD=ld.lld${VER}
VALUES="${LD} #$KERNEL_MAKE_ENV #$CLANG_VAL"

#########################
#===[ Smart Exports ]===#
#########################

if [ "$GCC_or_CLANG" -eq "1" ]
####-------####
#===[ GCC ]===#
####-------####
then

#===[ Most Editable ]===#

CC=aarch64-zyc-linux-gnu-gcc
GCC_PATH64=/root/kernels/aarch64-zyc-linux-gnu
GCC_PATH32=/root/kernels/arm-zyc-linux-gnueabi
GCC_BIN64=$GCC_PATH64/bin
GCC_BIN32=$GCC_PATH32/bin

#===[ Editable ]===#


GCC_PREF64=aarch64-zyc-linux-gnu
GCC_PREF32=arm-zyc-linux-gnueabi
GCC_PREFIX64=$GCC_PREF64-
GCC_PREFIX32=$GCC_PREF32-
GCC_LIB32=$GCC_PATH32/lib/$GCC_PREF32
GCC_LIB64=$GCC_PATH64/lib/$GCC_PREF64

#===[ Standart ]===#

GCC_LIBS=$GCC_LIB64:$GCC_LIB32
GCC_BINS=$GCC_BIN64:$GCC_BIN32
LD_LIBRARY_PATH=$GCC_LIBS:$LD_LIBRARY_PATH
PATH=$GCC_BINS:$PATH
CROSS_COMPILE=$GCC_PREFIX64
CROSS_COMPILE_ARM32=$GCC_PREFIX32

####---------####
#===[ Clang ]===#
####---------####
else

#===[ Most Editable ]===#

CC=clang${VER}
LLVM=llvm${VER}
CLANG_PATH1=/usr
CLANG_BIN=$CLANG_PATH1/lib/${LLVM}/bin
GCC_PATH64=/usr
GCC_PATH32=/usr
GCC_BIN64=$GCC_PATH64/bin
GCC_BIN32=$GCC_PATH32/bin

#===[ Editable ]===#

GCC_PREF64=aarch64-linux-gnu
GCC_PREF32=arm-linux-gnueabi
GCC_PREFIX64=aarch64-linux-gnu-
GCC_PREFIX32=arm-linux-gnueabi-
CLANG_LIB32=$CLANG_PATH1/lib/${LLVM}/lib
CLANG_LIB64=$CLANG_PATH1/lib/${LLVM}/lib64
GCC_LIB64=$GCC_PATH64/lib/$GCC_PREF64
GCC_LIB32=$GCC_PATH32/lib/$GCC_PREF32

#===[ Standart ]===#

GCC_BINS=$GCC_BIN64:$GCC_BIN32
GCC_LIBS=$GCC_LIB64:$GCC_LIB32
CLANG_LIBS=$CLANG_LIB64:$CLANG_LIB32
LD_LIBRARY_PATH=$CLANG_LIBS:$GCC_LIBS:$LD_LIBRARY_PATH
CLANG_PATH=${CLANG_BIN}
PATH=${CLANG_PATH}:${PATH}
CROSS_COMPILE=$GCC_PREFIX64
CLANG_TRIPLE=$GCC_PREFIX64
CROSS_COMPILE_ARM32=$GCC_PREFIX32
KBUILD_COMPILER_STRING="$(${CLANG_PATH}/clang --version | head -n 1 | perl -pe 's/\(http.*?\)//gs' | sed -e 's/  */ /g')"

fi

#======[ FUNCTIONS ]======#


buildtime()
	{
BUILD_END=$(date +"%s")
DIFF=$(($BUILD_END - $BUILD_START))
        echo -e "$blue Kernel compiled on $(($DIFF / 60)) minute(s) \
        and $(($DIFF % 60)) seconds$nocol"
	}

buildkernel()
	{
echo -e "$yellow\n ##============================================================================##"
echo -e " ##========================= Build Kernel From Source =========================##"
echo -e " ##============================================================================##$nocol\n"

if [ "$GCC_or_CLANG" -eq "1" ]
####-------####
#===[ GCC ]===#
####-------####
then
    	make $DEFCONFIG $COMPILE \
	CC=${CC} \
	PATH=${PATH} \
	CROSS_COMPILE=${CROSS_COMPILE} \
	CROSS_COMPILE_ARM32=${CROSS_COMPILE_ARM32} \
	ARCH=${ARCH} \
	O=${OUT_DIR} \
	INSTALL_MOD_PATH=. \
	INSTALL_MOD_DIR=. \
	${JOBS} \
	${VALUES} \
	${LOG}

####---------####
#===[ Clang ]===#
####---------####
else
	make all modules $DEFCONFIG $COMPILE \
	CC=${CC} \
	CLANG_PATH=${CLANG_PATH} \
	PATH=${PATH} \
	CLANG_TRIPLE=${CLANG_TRIPLE} \
	CROSS_COMPILE=${CROSS_COMPILE} \
	CROSS_COMPILE_ARM32=${CROSS_COMPILE_ARM32} \
	ARCH=${ARCH} \
	O=${OUT_DIR} \
	${JOBS} \
	INSTALL_MOD_PATH=. \
	INSTALL_MOD_DIR=. \
	$VALUES \
	${LOG}
fi
	}


buildkh()
	{
echo -e "$yellow\n ##============================================================================##"
echo -e " ##=========================== Build Kernel Headers ===========================##"
echo -e " ##============================================================================##$nocol\n"

if [ -f ${UN}/kernel-headers ]
then
rm -rf ${UN}/kernel-headers/kernel-headers/*
rm -rf ${UN}/kernel-headers-${CODENAME}.tar.xz
cp -r * ${UN}/kernel-headers/kernel-headers/
cd ${UN}/kernel-headers/kernel-headers/
else
git clone https://github.com/Anonym3310/kernel-headers --depth 1
rm ${UN}/kernel-headers/kernel-headers/*
cp -r * ${UN}/kernel-headers/kernel-headers/
cd ${UN}/kernel-headers/kernel-headers/
fi

if [ "$GCC_or_CLANG" -eq "1" ]
####-------####
#===[ GCC ]===#
####-------####
then
	make $DEFCONFIG prepare modules_prepare vdso_prepare \
	CC=${CC} \
	PATH=${PATH} \
	CROSS_COMPILE=${CROSS_COMPILE} \
	CROSS_COMPILE_ARM32=${CROSS_COMPILE_ARM32} \
	ARCH=${ARCH} \
    	${VALUES} \
	${JOBS}

####---------####
#===[ Clang ]===#
####---------####
else
   	make $DEFCONFIG prepare modules_prepare vdso_prepare \
	CC=${CC} \
	CLANG_PATH=${CLANG_PATH} \
	PATH=${PATH} \
	CLANG_TRIPLE=${CLANG_TRIPLE} \
	CROSS_COMPILE=${CROSS_COMPILE} \
	CROSS_COMPILE_ARM32=${CROSS_COMPILE_ARM32} \
	ARCH=${ARCH} \
	${JOBS} \
    	$VALUES
fi
exit
mkdir ${UN}/tmp
KN=$(find ${OUT_DIR}/lib/modules/ -name modules.*)

	cp -r arch/arm* Makefile ${OUT_DIR}/Module.symvers \
	${KN} ${OUT_DIR}/scripts/mod/modpost ${OUT_DIR}/scripts/genksyms/genksyms  \
	include scripts drivers/misc /${UN}/tmp

rm -rf *
cp -r ${UN}/tmp/* $PWD
mv modpost scripts/mod/
mv genksyms scripts/genksyms/
rm -rf ${UN}/tmp
mkdir arch
cp -r arm* arch
rm -rf arm*
mkdir drivers
cp -r misc drivers
rm -rf misc
cd $UN/
sudo dpkg-deb --build kernel-headers kernel-headers-${CODENAME}.deb
ls -l kernel-headers-${CODENAME}.deb

buildtime
	}


zipak3()
	{
echo -e "$yellow\n ##============================================================================##"
echo -e " ##===================== Creating A Flashable *.zip Archive ===================##"
echo -e " ##============================================================================##$nocol\n"

cd ${UN}/${NKD}
rm -rf ${ANYKERNEL_DIR}
cp ${UN}/${ANYKERNEL_DIR} ${UN}/${NKD} -r

if [ "$GCC_or_CLANG" -eq "1" ]
####-------####
#===[ GCC ]===#
####-------####
then
	make modules_install \
        CC=${CC} \
        PATH=${PATH} \
        CROSS_COMPILE=${CROSS_COMPILE} \
        CROSS_COMPILE_ARM32=${CROSS_COMPILE_ARM32} \
        ARCH=${ARCH} \
        O=${OUT_DIR} \
        INSTALL_MOD_DIR=. \
	${JOBS}

 	KERNEL_NAME=$(make kernelrelease \
        CC=${CC} \
	PATH=${PATH} \
        CROSS_COMPILE=${CROSS_COMPILE} \
        CROSS_COMPILE_ARM32=${CROSS_COMPILE_ARM32} \
        ARCH=${ARCH} \
        O=${OUT_DIR} \
        INSTALL_MOD_PATH=. \
	${VALUES} \
	${JOBS})

####---------####
#===[ Clang ]===#
####---------####
else
	make modules_install \
        CC=${CC} \
        CLANG_PATH=${CLANG_PATH} \
	PATH=${PATH} \
        CLANG_TRIPLE=${CLANG_TRIPLE} \
        CROSS_COMPILE=${CROSS_COMPILE} \
        CROSS_COMPILE_ARM32=${CROSS_COMPILE_ARM32} \
        ARCH=${ARCH} \
        O=${OUT_DIR} \
        INSTALL_MOD_DIR=. \
	${JOBS} \
	$VALUES

	KERNEL_NAME=$(make kernelrelease \
        CC=${CC} \
        CLANG_PATH=${CLANG_PATH} \
	PATH=${PATH} \
        CLANG_TRIPLE=${CLANG_TRIPLE} \
        CROSS_COMPILE=${CROSS_COMPILE} \
        CROSS_COMPILE_ARM32=${CROSS_COMPILE_ARM32} \
        ARCH=${ARCH} \
        O=${OUT_DIR} \
        ${JOBS} \
	$VALUES)
fi

#===[ COPYNG ]===#

cd ${UN}/${NKD}/${OUT_DIR}
rm $(find lib/modules/ -name build)
rm $(find lib/modules/ -name source)
cp lib/modules/ -r ${UN}/${NKD}/${ANYKERNEL_DIR}/modules/system/lib/
cd firmware
#cp $(find -name *.bin) -r --parents ${UN}/${NKD}/${ANYKERNEL_DIR}/modules/system/etc/firmware
#cp $(find -name *.fw) -r --parents ${UN}/${NKD}/${ANYKERNEL_DIR}/modules/system/etc/firmware
cd ../../

#===( EDITABLE )===#

cp $(find -name ${IMAGE}) ${ANYKERNEL_DIR}/
cp $(find -name dtb) ${ANYKERNEL_DIR}/
cp $(find -name dtbo.img) ${ANYKERNEL_DIR}/

#===[ ZIPPING ]===#

cd ${UN}/${NKD}
	if [ -f AK3 ]
	then
	cp -r AK3/* ${ANYKERNEL_DIR}
	fi
cd ${ANYKERNEL_DIR}
	zip -r -9 AkameKernel-${CODENAME}-${ANDROID}-$(date +%d-%m-%y).zip * \
	-x .git README.md *placeholder

#===[ TIME BUILD ]===#

buildtime
	}

ziptest()
	{
if [ -f ${UN}/${NKD}/${OUT_DIR}/arch/arm64/boot/${IMAGE} ]
then
zipak3
else
echo ""
echo $red"Compilation failed"
echo $red"$IMAGE not builded"
echo ""
exit
fi
	}



######################
#=[ START OF BUILD ]=#
######################

if [ "$ONLY_BUILD_AN" -eq "1" ]
then

if [ "$ONLY_BUILD_KH" -eq "1" ]
then
buildkernel

if [ "$BUILD_KH" -eq "1" ]

#####################
#===[ Biuild KH ]===#
#####################
then
buildkh
ziptest

#################
#===[Skip KH]===#
#################
else
ziptest
fi

#===[ END BUILD ]===#

#===[ ONLY BUILD KERNEL HEADERS ]===#
else
buildkh
fi

################################
#===[ ONLY_BUILD_ANYKERNEL ]===#
################################
else
ziptest
fi
