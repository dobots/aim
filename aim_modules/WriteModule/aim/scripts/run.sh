#!/bin/bash

####################################################################################
# Warning! Do not "echo" anything in this script, it will end up in the generated
# header file(s)!
####################################################################################

# Check if we have at least the backend as argument
if [[ "$1" == "" ]]
then
	echo "No args supplied! Use idl file + backend as argument"
	exit 1
fi

# following scripts set paths
default_script=../default.sh
local_script=../local.sh

if [ -a $default_script ]; then
	. ../default.sh
fi
if [ -a $local_script ]; then
	. ../local.sh
fi

idl_file=$1
backend=$2
backend_path=$3

# The only output is from omniidl
omniidl -p${backend_path} -b${backend} "${idl_file}"

####################################################################################
# Warning! Do not "echo" anything in this script, it will end up in the generated
# header file(s)!
####################################################################################

