#!/bin/bash

# Setup for autotools
# see also blackfin.mk which might be more appropriate for cmake

####################################################################################
# Path and prefix to the Blackfin compiler specific to uClinux
####################################################################################

# The blackfin compiler to be used
COMPILER=bfin-linux-uclibc
COMPILER_PREFIX=$COMPILER-
TARGET=$COMPILER_PREFIX

# The target platform
HOST=bfin-elf

# Default toolchain location when installed by aptitude from analog PPA
BFIN_PATH=/opt/uClinux/$COMPILER/bin

# Make it easy to find the cross-compiler
export PATH="$PATH:$BFIN_PATH"

# The target installation 
export PREFIX_PATH=/data/blackfin

####################################################################################
# Where can the libraries and the include dirs be found?
####################################################################################

RUNTIME_PATH=/opt/uClinux/$COMPILER/$COMPILER/runtime

INCLUDE_PATHS="-I${RUNTIME_PATH}/usr/include/ -I${PREFIX_PATH}/usr/include -I${PREFIX_PATH}/usr/local/include"
LIBRARY_PATHS="-L${PREFIX_PATH}/usr/lib -L${PREFIX_PATH}/usr/local/lib"

####################################################################################
# Compilation flags
####################################################################################

# Default compiler/assembler options
COMPILER_FLAGS+="-O3 -mcpu=bf561"

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
