#!/bin/bash

# Check if we have at least the backend as argument
if [[ "$1" == "" ]]
then
	echo "No args supplied! Use backend as argument"
	exit 1
fi

backend=$1
backend_path=$2

default_script=../default.sh
local_script=../local.sh

if [ -a $default_script ]; then
	echo "Source default script for environmental variables"
	. ../default.sh
fi
if [ -a $local_script ]; then
	echo "Source local script for environmental variables"
	. ../local.sh
fi

echo "We will use backend \"$backend\""

IDL_FILE="../idl/ReadModule.idl"
RESULT="../inc/ReadModule.h"
RESULT_TEMP="../inc/ReadModule.h.temp"

OMNIIDL=`which omniidl`
if [[ $OMNIIDL == "" ]]; then
	echo "[!] There is no omniidl installed!"
	rm -f $RESULT.invalid
	if [ -a $RESULT ]; then
		mv -f $RESULT $RESULT.invalid
	fi
	exit 2
fi

# Generate temporary header file
./run.sh "${IDL_FILE}" "${backend}" "${backend_path}" > "${RESULT_TEMP}" 

# Copy it to normal header file only if they are different
if ! diff -q $RESULT_TEMP $RESULT > /dev/null ; then
	mv -f $RESULT_TEMP $RESULT
else
	rm $RESULT_TEMP
fi

# This ensures a remote repository can obtain a header file, but it does not
# need to be commited again, just because the same code is regenerated with 
# a different time stamp.
