#!/bin/make

# Author: Anne C. van Rossum
# Date: Aug. 04, 2011

# This include file should define BACKEND
include local.mk

INSTALL_PATH:=$(DESTDIR)/etc/aim/rur-builder/backends

CURRENT_MODULE_PATH:=$(CURDIR)
ROS_PACKAGE_PATH:=$(ROS_PACKAGE_PATH):$(CURRENT_MODULE_PATH)

ROS_FLAG=-DBUILD_ROS:bool=off
YARP_FLAG=-DBUILD_YARP:bool=off
FLAGS=$(ROS_FLAG) $(YARP_FLAG) -DBUILD_$(BACKEND):bool=on

all:
	@cd scripts && ./example_sum.sh; if [ $$? -eq 2 ]; then echo "Error: Cannot make because no proper header file generated!"; exit 2; fi
#	@echo "Ros package path: $(ROS_PACKAGE_PATH)"
	@mkdir -p build
	@echo "Run cmake with the flags: $(FLAGS)"
	@cd build && cmake $(CMAKE_FLAGS) .. $(FLAGS)
	@cd build && make

install:
	@echo "The installation will be in the path: $(INSTALL_PATH)"
	@mkdir -p $(INSTALL_PATH)
	@install backends/*.py $(INSTALL_PATH)

clean:
	cd build && make clean
