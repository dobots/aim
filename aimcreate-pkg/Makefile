#!/bin/make

# Author: Anne C. van Rossum
# Date: Aug. 04, 2011

# This include file should define BACKEND
#include local.mk

RUR_TEMPLATES=/var/lib/rur

# We don't need to build anything
all: 
	@echo ${USER} > user.txt

# We only install (need to be sudo)
install:
	@sh install.sh ${RUR_TEMPLATES}

