#!/bin/make

# Author: Anne C. van Rossum
# Date: Apr. 5, 2013

build_folder=build_standard

all: 
	@mkdir -p $(build_folder)
	cd $(build_folder) && cmake $(CMAKE_FLAGS) ..
	cd $(build_folder) && make

clean:
	cd $(build_folder) && make clean
	rm -f $(build_folder)/CMakeCache.txt
