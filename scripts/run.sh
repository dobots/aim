#!/bin/bash

export PATH=$PATH:/opt/omniORB-4.1.6/bin

backend=$2

#echo "We will run omniidl -p../backends -b${backend} \"$1\""
#echo
omniidl -p../backends -b${backend} "$1"

