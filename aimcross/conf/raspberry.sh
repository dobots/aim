#!/bin/bash

# Setup for autotools
# see also raspberry.mk which might be more appropriate for cmake

####################################################################################################
# Configuration
####################################################################################################

# Installation paths, the bin path is to store the application itself, the share path to store 
# additional details such as helper python files, icons, translations. The config path to store 
# configuration details, such as starting at booting.
AIM_SHARE_PATH=/usr/share/aim

# Update paths by prepending it with DESTDIR so installation via the Ubuntu PPA works properly. This
# causes a duplicate "//" when $DESTDIR is empty, which is no problem at all, just does not look so
# nice.
AIM_SHARE_PATH="$DESTDIR/$AIM_SHARE_PATH"

# Default configuration files
AIM_CONFIG_COLOR=$AIM_CONFIG_PATH/color.sh

####################################################################################################
# Tweaks and general configuration details
####################################################################################################

source $AIM_CONFIG_COLOR

####################################################################################
# Path and prefix to this specific cross-compiler
####################################################################################

COMPILER=arm-linux-gnueabihf
COMPILER_VERSION=4.7.2

# The raspberry compiler to be used
COMPILER_PREFIX=$COMPILER-
TARGET=$COMPILER_PREFIX

# The target platform
#HOST=arm-linux-gnueabihf

# Default toolchain location
CROSSCOMPILER_PATH=/opt/raspberrypi/arm-bcm2708/gcc-linaro-$COMPILER-raspbian

# Default locations for header files 
INCLUDE_PATH[0]=${CROSSCOMPILER_PATH}/$COMPILER/include
INCLUDE_PATH[1]=${CROSSCOMPILER_PATH}/$COMPILER/libc/include
INCLUDE_PATH[2]=${CROSSCOMPILER_PATH}/$COMPILER/libc/include/$COMPILER
INCLUDE_PATH[3]=${CROSSCOMPILER_PATH}/lib/gcc/$COMPILER/$COMPILER_VERSION/include-fixed
INCLUDE_PATH[4]=${CROSSCOMPILER_PATH}/lib/gcc/$COMPILER/$COMPILER_VERSION/include
INCLUDE_PATH[5]=${CROSSCOMPILER_PATH}/lib/gcc/$COMPILER/$COMPILER_VERSION/finclude
INCLUDE_PATH[6]=${CROSSCOMPILER_PATH}/$COMPILER/include/c++/$COMPILER_VERSION
INCLUDE_PATH[7]=${CROSSCOMPILER_PATH}/$COMPILER/include/c++/$COMPILER_VERSION/$COMPILER

# Default locations for library paths
LIB_PATH[0]=${CROSSCOMPILER_PATH}/$COMPILER/lib
LIB_PATH[1]=${CROSSCOMPILER_PATH}/$COMPILER/libc/lib
LIB_PATH[2]=${CROSSCOMPILER_PATH}/$COMPILER/libc/lib/$COMPILER
LIB_PATH[3]=${CROSSCOMPILER_PATH}/lib/gcc/$COMPILER/$COMPILER_VERSION
LIB_PATH[4]=${CROSSCOMPILER_PATH}/$COMPILER/lib
LIB_PATH[5]=${CROSSCOMPILER_PATH}/$COMPILER/libc/lib/$COMPILER

# Make it easy to find the cross-compiler
export PATH="$PATH:$CROSSCOMPILER_PATH/bin"

# The target installation 
export PREFIX_PATH=/data/raspberry

INCLUDE_PATH[8]=${PREFIX_PATH}/usr/include
INCLUDE_PATH[9]=${PREFIX_PATH}/usr/local/include

LIB_PATH[6]=${PREFIX_PATH}/usr/lib
LIB_PATH[7]=${PREFIX_PATH}/usr/local/lib

####################################################################################
# Where can the libraries and the include dirs be found?
####################################################################################

INCLUDE_PATHS=""
for i in ${INCLUDE_PATH[@]}; do
	INCLUDE_PATHS="$INCLUDE_PATHS -I${i}"
done

LIBRARY_PATHS=""
for i in ${LIB_PATH[@]}; do
	LIB_PATHS="$LIB_PATHS -L${i}"
done

####################################################################################
# Compilation flags
####################################################################################

# Default compiler/assembler options

#ASMFLAGS=
#LDFLAGS += -lpthread

export CC=${COMPILER_PREFIX}gcc
export CXX=${COMPILER_PREFIX}g++
export CSIZE=${COMPILER_PREFIX}size
export ASM=${COMPILER_PREFIX}as
export STRIP=${COMPILER_PREFIX}strip

export CFLAGS="$COMPILER_FLAGS ${INCLUDE_PATHS}"
export CXXFLAGS=${CFLAGS}
export CPPFLAGS=${CFLAGS}
export LDFLAGS="${LIBRARY_PATHS}"

####################################################################################
# Perform some checks
####################################################################################

COMPILER_EXISTS=$(which $CXX)
if [[ $COMPILER_EXISTS == "" ]]; then
	msg_error "Compiler $CXX cannot be found"
else
	msg_info "Use compiler $COMPILER_EXISTS"
fi

####################################################################################################
# Information for user
####################################################################################################

msg_info "We will use cross-compiler $CC for C, or $CXX for C++"
msg_info "We will assume you installed the one from the raspberry github"
msg_info "cd /opt && git clone git://github.com/raspberrypi/tools.git raspberrypi"

