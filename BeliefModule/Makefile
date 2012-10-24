#!/bin/make

# Author: Anne C. van Rossum
# Date: Jul. 20, 2012

all: 
	@mkdir -p build
	cd build && cmake $(CMAKE_FLAGS) -DBUILD_EIGEN:BOOL=ON ..
	cd build && make

clean:
	cd build && make clean
	rm -f build/CMakeCache.txt

# Be careful with this one, however, it's convenient before uploading to a
# repository, because it removes all generated files for sure
clean_thorough:
	rm -rf build
	rm -f inc/config.h
	rm -f aim/inc/*.h

