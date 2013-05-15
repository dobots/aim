#!/bin/make

# Author: Anne C. van Rossum
# Date: Apr. 5, 2013

build_folder=build_standard

#folders := $(shell ls -1 | grep build)
folders := $(shell find -mindepth 1 -maxdepth 1 -type d -print)
#folders = build_standard build_blackfin

thisfolder=$(pwd)

all: 
	@mkdir -p $(build_folder)
	cd $(build_folder) && cmake $(CMAKE_FLAGS) ..
	cd $(build_folder) && make

clean:
	cd $(build_folder) && make clean
	rm -f $(build_folder)/CMakeCache.txt

clean_all:
	@for f in $(folders); do \
		echo "Clean folder $${f}"; \
#		cd "$${f}" && make clean; \
		cd "$${f}" && pwd; \
		cd ..; \
	done

list_all:
	@mkdir -p "test"
	@cd "test" && pwd
	@pwd
	@for f in $(folders); do \
		cd "$${f}" && pwd; \
		pwd; \
		cd ..; \
	done


#	cd $(build_folder) && make clean
#	rm -f $(build_folder)/CMakeCache.txt
