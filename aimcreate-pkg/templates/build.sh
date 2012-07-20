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

IDL_FILE="../idl/TemplateModule.idl"
RESULT="../aim/TemplateModule.h"

./run.sh "${IDL_FILE}" "${backend}" "${backend_path}" > "${RESULT}" 


