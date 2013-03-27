#!/bin/make

# Author: Anne C. van Rossum
# Date: Mar. 27, 2013

RUR_BIN_PATH=/usr/bin
RUR_SHARE_PATH=/usr/share/rur
RUR_TEMPLATE_PATH=$(RUR_SHARE_PATH)/templates
RUR_BACKENDS_PATH=$(RUR_SHARE_PATH)/backends
RUR_CONFIG_PATH=/etc/rur

BACKENDS_INSTALL_PATH:=$(DESTDIR)/$(RUR_BACKENDS_PATH)

all:
	@echo "[#] Just python and bash scripts. Nothing needs to be compiled"

install:
	@echo "[#] The installation will be in the path: $(BACKENDS_INSTALL_PATH)"
	@mkdir -p $(BACKENDS_INSTALL_PATH)/helper
	@cp backends/*.py $(BACKENDS_INSTALL_PATH)
	@cp backends/helper/*.py $(BACKENDS_INSTALL_PATH)/helper
	@install rur-builder ${RUR_BIN_PATH}

clean:
	@echo "[#] Just python and bash scripts. Just removing the .pyc files"
	@rm backends/*.pyc
	@rm backends/helper/*.pyc
	
uninstall:
	@echo "[#] Removing files from path: $(BACKENDS_INSTALL_PATH)"
	@rm $(BACKENDS_INSTALL_PATH)/helper/*.py
	@rm $(BACKENDS_INSTALL_PATH)/*.py
	@rmdir $(BACKENDS_INSTALL_PATH)/helper
	@rmdir $(BACKENDS_INSTALL_PATH)
	@install ${RUR_BIN_PATH}/rur-builder 

