#!/bin/make

# Author: Anne C. van Rossum
# Date: Jul. 20, 2012

all: 
	@mkdir -p build
	cd build && cmake $(CMAKE_FLAGS) ..
	cd build && make

clean:
	cd build && make clean
	rm -f build/CMakeCache.txt

install:
	install build/zmqconnect $(DESTDIR)/usr/bin

uninstall:
	rm $(DESTDIR)/usr/bin/zmqconnect
