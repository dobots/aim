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

# We need a reference to the path without the destination dir
RUR_BACKENDS_PATH_WITHOUT_DESTDIR=${RUR_BACKENDS_PATH}

# Prepend with destination dir if DESTDIR is present
RUR_SHARE_PATH="$DESTDIR/$RUR_SHARE_PATH"
RUR_TEMPLATE_PATH="$DESTDIR/$RUR_TEMPLATE_PATH"
RUR_BACKENDS_PATH="$DESTDIR/$RUR_BACKENDS_PATH"
RUR_CONFIG_PATH="$DESTDIR/$RUR_CONFIG_PATH"

# Configuration files (the .conf file is convenient for bash scripts, the .cmake one for cmake)
RUR_CONFIG_FILE_BACKENDS=${RUR_CONFIG_PATH}/backends.conf
RUR_CONFIG_FILE_BACKENDS_CMAKE=${RUR_CONFIG_PATH}/backends.cmake

####################################################################################################
# Checks
####################################################################################################

# First, check if we have sudo rights
if [ ! `id --user` -eq 0 ]; then
	echo "[#] Sorry, super user rights needed (run with sudo)"
	exit 1
fi

####################################################################################################
# Load configuration details
####################################################################################################

# Create paths if not present
mkdir --parents ${RUR_SHARE_PATH}
mkdir --parents ${RUR_TEMPLATE_PATH}
mkdir --parents ${RUR_CONFIG_PATH}

# If RUR_CONFIG_FILE_BACKENDS exists, get configuration data from it
if [ -e ${RUR_CONFIG_FILE_BACKENDS} ]; then
	source $RUR_CONFIG_FILE_BACKENDS
else
	echo "[#] Write backends path to configuration file: ${RUR_CONFIG_FILE_BACKENDS}"
	echo "RUR_BACKENDS_PATH=\"$RUR_BACKENDS_PATH_WITHOUT_DESTDIR\"" >> $RUR_CONFIG_FILE_BACKENDS
fi

echo "[#] Use backends path: ${RUR_BACKENDS_PATH_WITHOUT_DESTDIR}"

echo "[#] Write backends path to cmake configuration file: ${RUR_CONFIG_FILE_BACKENDS_CMAKE}"
echo "SET(BACKENDS_PATH $RUR_BACKENDS_PATH_WITHOUT_DESTDIR)" > "${RUR_CONFIG_FILE_BACKENDS_CMAKE}"

####################################################################################################
# Start
####################################################################################################

# Copy to system-wide directories
cp --recursive templates/* ${RUR_TEMPLATE_PATH}

# Remove files we do not need per project (and obsolete ones)
echo "[*] Remove temporary and old files from template folder"
rm -f ${RUR_TEMPLATE_PATH}/local.mk
rm -f ${RUR_TEMPLATE_PATH}/rur.mk
rm -f ${RUR_TEMPLATE_PATH}/rur_header.mk

# Copy aimcreate-pkg itself
echo "[#] Copy aimcreate-pkg to ${RUR_BIN_PATH}"
install aimcreate-pkg $DESTDIR/${RUR_BIN_PATH}

echo "[#] Files in template directory: ${RUR_TEMPLATE_PATH}"
ls ${RUR_TEMPLATE_PATH}

echo "[#] End of aimcreate-pkg installation"

