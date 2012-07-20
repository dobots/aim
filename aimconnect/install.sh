#!/bin/bash

PROGRAM="aimconnect"

if [ `id -u` -eq 0 ]
then
	echo "Install $PROGRAM"
else
	echo "Sorry, super user rights needed (run with sudo)"
	exit 1
fi

cp $PROGRAM /usr/bin
