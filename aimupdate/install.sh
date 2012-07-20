#!/bin/bash

# Check if we have sudo rights
if [ `id -u` -eq 0 ]; then
	echo "Install aimupdate"
else
	echo "Sorry, super user rights needed (run with sudo)"
	exit 1
fi

cp aimupdate /usr/bin

