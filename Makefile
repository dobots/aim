#!/bin/make 

# Thanks to http://www.gnu.org/software/make/manual/html_node/Phony-Targets.html
# Author: A.C. van Rossum
# Date: 2013
# License: LGPL v.3
# Company: Distributed Organisms B.V.

SUBDIRS=aimlist aimcreate-pkg aimupdate aimcopy aimrun aimconnect aimlogin \
	aimregister aimstop

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

all: $(SUBDIRS)

install:
	for dir in $(SUBDIRS); do \
		$(MAKE) install -C $$dir; \
	done

.PHONY: subdirs $(SUBDIRS)
