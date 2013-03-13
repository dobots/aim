#!/bin/make

targetdir=$(DESTDIR)/usr/bin/zmqserver-dir

all:
	@echo "This is a server based on node.js. It does not need to be compiled"
	@echo "run \"npm install -d\" in this directory if you want to update the packages in the node_modules directory"

install:
	mkdir --parents $(targetdir)
	cp --recursive node_modules $(targetdir)
	cp server.js $(targetdir)
	install scripts/zmqserver $(DESTDIR)/usr/bin

uninstall:
	@echo "We will forcefully remove everything from $(targetdir)!"
	rm --recursive --force $(targetdir)
	rm $(DESTDIR)/usr/bin/zmqserver

