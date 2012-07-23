#!/bin/bash

RUR_TEMPLATES="$1"

# Check if we have sudo rights
if [ `id -u` -eq 0 ]; then
	echo "Remove everything in ${RUR_TEMPLATES}"
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


# Remove all files in ${RUR_TEMPLATES}
rm ${RUR_TEMPLATES}/*.sh
rm ${RUR_TEMPLATES}/*.idl
rm ${RUR_TEMPLATES}/*.cpp
rm ${RUR_TEMPLATES}/*.cmake
rm ${RUR_TEMPLATES}/*.txt
rm ${RUR_TEMPLATES}/*.parent
rm ${RUR_TEMPLATES}/*.xml
rm ${RUR_TEMPLATES}/*.mk
rm ${RUR_TEMPLATES}/cmake/*
rmdir ${RUR_TEMPLATES}/cmake
rmdir ${RUR_TEMPLATES}

# Copy aimcreate-pkg itself
echo "Remove aimcreate-pkg in /usr/bin"
rm /usr/bin/aimcreate-pkg


