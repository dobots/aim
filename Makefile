#!/bin/make 

# Thanks to http://www.gnu.org/software/make/manual/html_node/Phony-Targets.html
# Author: A.C. van Rossum
# Date: 10 March, 2013
# License: LGPL v.3
# Company: Distributed Organisms B.V.

# Space-separated list of subdirectories that have to be made, the construction here is that
# make <TAB> will allow bash-completion for individual directories.

subdirs=aimtools rur-builder zmqserver aim_modules

.PHONY: subdir
subdir: $(subdirs)

.PHONY: $(subdirs) 
$(subdirs): 
	$(MAKE) -C $@

.PHONY: all
all: $(subdirs)

install:
	for dir in $(subdirs); do \
		$(MAKE) install -C $$dir; \
	done

clean:
	echo "Not implemented clean targets yet"

help:
	@echo "Type make <TAB> to make a sub-project"
	@echo "Type sudo make install <TAB> to install a sub-project"

