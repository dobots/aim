#!/bin/make -f

# Setup for inclusion in cmake build systems
# see also blackfin.sh which might be more appropriate for autotools

####################################################################################
# Path and prefix to the Blackfin compiler specific to uClinux
####################################################################################

# Default toolchain location when installed by aptitude from analog PPA
BFIN_PATH=/opt/uClinux/bfin-uclinux/bin

COMPILER=bfin-linux-uclibc
COMPILER_PREFIX=$(COMPILER)-
TARGET=$(COMPILER_PREFIX)

PATH:=$(PATH):/opt/uClinux/$(COMPILER)/bin/

####################################################################################
# Where can the libraries be found?
####################################################################################

# Default compiler/assembler options
CXXFLAGS+=-O3 -mcpu=bf561
ASMFLAGS=
CFLAGS+=-O3 -mcpu=bf561

#LDFLAGS += -lpthread

CC=$(COMPILER_PREFIX)gcc
CXX=$(COMPILER_PREFIX)g++
CSIZE=$(COMPILER_PREFIX)size
ASM=$(COMPILER_PREFIX)as
STRIP=$(COMPILER_PREFIX)strip

