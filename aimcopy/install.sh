#!/bin/bash

program=aimcopy

# Check if we have sudo rights
if [ `id -u` -eq 0 ]; then
	echo "Install $program"
else
	echo "Sorry, super user rights needed (run with sudo)"
	exit 1
fi

targetdir=$DESTDIR/usr/bin
mkdir -p $targetdir
install $program $targetdir

