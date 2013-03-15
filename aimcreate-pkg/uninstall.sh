#!/bin/bash

####################################################################################################
# Configuration
####################################################################################################

# Installation paths
RUR_BIN_PATH=/usr/bin
RUR_SHARE_PATH=/usr/share/rur
RUR_TEMPLATE_PATH=$RUR_SHARE_PATH/templates
RUR_BACKENDS_PATH=$RUR_SHARE_PATH/backends
RUR_CONFIG_PATH=/etc/rur

# Configuration files
RUR_CONFIG_FILE_BACKENDS=${RUR_CONFIG_PATH}/backends.conf
RUR_CONFIG_FILE_BACKENDS_CMAKE=${RUR_CONFIG_PATH}/backends.cmake

# Prepend with destination dir if DESTDIR is present
RUR_SHARE_PATH="$DESTDIR/$RUR_SHARE_PATH"
RUR_TEMPLATE_PATH="$DESTDIR/$RUR_TEMPLATE_PATH"
RUR_BACKENDS_PATH="$DESTDIR/$RUR_BACKENDS_PATH"
RUR_CONFIG_PATH="$DESTDIR/$RUR_CONFIG_PATH"

####################################################################################################
# Checks
####################################################################################################

# First, check if we have sudo rights
if [ ! `id --user` -eq 0 ]; then
	echo "[#] Sorry, super user rights needed (run with sudo)"
	exit 1
fi

read -p "[#] Remove everything in ${RUR_TEMPLATE_PATH} [y/N]?: " goon
if [ "${goon}" != "y" ]; then
	echo "[#] Sorry, type \"y\" next time if you want to uninstall"
	exit 0
fi

####################################################################################################
# Load configuration details
####################################################################################################

# Remove all files in ${RUR_TEMPLATE_PATH}, here we are careful not to executed any "rm -rf" 
# command. However, we also do not want errors popping up because a directory does not exist. Hence,
# the mkdir commands before the rmdir commands.
rm -f ${RUR_TEMPLATE_PATH}/cmake/*
mkdir -p ${RUR_TEMPLATE_PATH}/cmake
rmdir ${RUR_TEMPLATE_PATH}/cmake
rm -f ${RUR_TEMPLATE_PATH}/*
mkdir -p ${RUR_TEMPLATE_PATH}
rmdir ${RUR_TEMPLATE_PATH}

# Copy aimcreate-pkg itself
echo "[#] Remove aimcreate-pkg in ${RUR_BIN_PATH}"
rm -f ${RUR_BIN_PATH}/aimcreate-pkg


