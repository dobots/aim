#!/bin/make

# Author: Anne C. van Rossum
# Date: Mar. 15, 2013

# We don't need to build anything
.PHONY: all
all: 
	@echo ${USER} > user.txt

# We just run an install script (sudo is enforced in the script)
.PHONY: install
install:
	@sh install.sh

# We just run an install script (sudo is enforced in the script)
.PHONY: uninstall
uninstall:
	@sh uninstall.sh

