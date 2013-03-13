#!/bin/make 

# Thanks to http://www.gnu.org/software/make/manual/html_node/Phony-Targets.html
# Author: A.C. van Rossum
# Date: 10 March, 2013
# License: LGPL v.3
# Company: Distributed Organisms B.V.

# Space-separated list of subdirectories that have to be made, the construction here is that
# make <TAB> will allow bash-completion for individual directories.

.DEFAULT_GOAL=all

subdirs=*Module

.PHONY: subdir
subdir: $(subdirs)

.PHONY: $(subdirs) 
$(subdirs): 
	$(MAKE) -C $@

.PHONY: all
all: inform 
ifdef force 
	$(MAKE) force_all 
endif

.PHONY: force_all
force_all: $(subdirs)

install:
	@echo "Even less than compiling all the modules at once, we will not install them all at once"
	@echo "Most modules are not meant to be run as root and it's better practise to sandbox them in a specific directory"
	@echo "Do also not forget to run aimregister on compiled modules to have other aim tools find them"
#	for dir in $(subdirs); do \
#		$(MAKE) install -C $$dir; \
#	done

clean:
	echo "Not implemented clean targets yet"

help:
	@echo "Type make <TAB> to make a sub-project"
	@echo "Type sudo make install <TAB> to install a sub-project"

.PHONY: inform
inform:
	@echo "By default there is no all target. It is too much work to get each module all the time compiled correctly."
	@echo "The source of the modules is however important. Use the cmake build process to compile individual modules."
	@echo "To actually compile all of them use: make force=1, or make force_all, or make all force=1"


