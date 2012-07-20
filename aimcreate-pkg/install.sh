#!/bin/bash

RUR_TEMPLATES="$1"

# Check if we have sudo rights
if [ `id -u` -eq 0 ]; then
	echo "Copy templates to ${RUR_TEMPLATES}"
else
	echo "Sorry, super user rights needed (run with sudo)"
	exit 1
fi

# Configuration directory for RUR
RUR_HOME=~/.rur
USER_FILE=user.txt

mkdir -p ${RUR_HOME}

if [ -a ${USER_FILE} ]; then
	RUSER=`cat ${USER_FILE}`
	if [ -e ${RUSER} ]; then
		chown $RUSER:$RUSER ${RUR_HOME}
	fi
fi


# File in which we store path towards backends
BACKENDS_PATH_FILE="${RUR_HOME}/backends_path"

# The different possible backends
RUR_MK=templates/rur.mk
cp templates/rur_header.mk $RUR_MK

# Tentative backends directory
BACKENDS_PATH="$HOME/workspace/rur-builder/backends"

# Retrieve backends directory from file if it exists
if [ -e ${BACKENDS_PATH_FILE} ]; then
	BACKENDS_PATH=`cat $BACKENDS_PATH_FILE`
fi

# Ask user for omniidl backends directory if not obtained yet
if [ -e /usr/bin/zenity ]; then
	BACKENDS_PATH=`zenity --entry --text "Please enter the omniidl backends path" --entry-text "$BACKENDS_PATH" --title "Enter omniidl backends path"`
	echo "$BACKENDS_PATH" > "$BACKENDS_PATH_FILE" 
else
	echo "No zenity installed, you will need to set RUR_BACKENDS=... yourself in ${RUR_TEMPLATES}/local.mk"
fi

echo "RUR_BACKENDS=$BACKENDS_PATH" >> ${RUR_MK}

# Copy to system-wide directories
rsync -avzuL templates/ --exclude="*.svn" ${RUR_TEMPLATES}

# Remove files we do not need per project (and obsolete ones)
rm -f ${RUR_TEMPLATES}/local.mk
rm -f ${RUR_TEMPLATES}/rur.mk
rm -f ${RUR_TEMPLATES}/rur_header.mk

# Copy aimcreate-pkg itself
echo "Copy aimcreate-pkg to /usr/bin"
cp aimcreate-pkg /usr/bin

cp -f ${RUR_MK} ${RUR_HOME}/rur.mk

