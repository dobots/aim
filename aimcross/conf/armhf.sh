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

COMPILER_VERSION=4.7

CC_DEB=gcc-$COMPILER
CXX_DEB=g++-$COMPILER
BINUTILS_DEB=binutils-$COMPILER

LIBC_DEB=libc6-armhf-cross

# The raspberry compiler to be used
COMPILER_PREFIX=$COMPILER-
TARGET=$COMPILER_PREFIX

# The target platform
HOST=arm-linux-gnueabihf

# Default toolchain location
CROSSCOMPILER_PATH=

# Default location for header files and (standard) libraries for this target
RUNTIME_PATH=/usr/$COMPILER

# Make it easy to find the cross-compiler
export PATH="$PATH:$CROSSCOMPILER_PATH"

# The target installation 
export PREFIX_PATH=/data/raspberry

####################################################################################
# Where can the libraries and the include dirs be found?
####################################################################################

INCLUDE_PATHS=" -I${RUNTIME_PATH}/include -I${PREFIX_PATH}/usr/include -I${PREFIX_PATH}/usr/local/include"
LIBRARY_PATHS=" -L${RUNTIME_PATH}/lib -L${PREFIX_PATH}/usr/lib -L${PREFIX_PATH}/usr/local/lib"

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

####################################################################################################
# Information for user
####################################################################################################

msg_info "We will use cross-compiler $CC for C, or $CXX for C++"
msg_info "On Ubuntu this will be installed by apt-get install $CC_DEB or $CXX_DEB"
msg_info "Also do not forget binutils: apt-get install $BINUILS_DEB for the -ld, -strip, etc. tools"
msg_info "And, do not forget standard C/C++ libraries, for example (give or take compiler version):"
msg_info "apt-get install $LIBC_DEB for std::cout, std::vector, etc"
msg_info "Make sure you have enabled the \"universe\" repository"

