#!/bin/bash

####################################################################################################
# Configuration
####################################################################################################

# Installation paths, the bin path is to store the application itself, the share path to store 
# additional details such as helper python files, icons, translations. The config path to store 
# configuration details, such as starting at booting.
AIM_BIN_PATH=/usr/bin
AIM_SHARE_PATH=/usr/share/aim
AIM_CONFIG_PATH=/etc/aim

# Update paths by prepending it with DESTDIR so installation via the Ubuntu PPA works properly. This
# causes a duplicate "//" when $DESTDIR is empty, which is no problem at all, just does not look so
# nice.
AIM_BIN_PATH="$DESTDIR/$AIM_BIN_PATH"
AIM_SHARE_PATH="$DESTDIR/$AIM_SHARE_PATH"
AIM_CONFIG_PATH="$DESTDIR/$AIM_CONFIG_PATH"

####################################################################################################
# Checks
####################################################################################################

targetdir=$AIM_CONFIG_PATH
mkdir -p $targetdir
cp *.sh $targetdir

