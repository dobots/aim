#!/bin/bash

echo "We make the example \"sum\"" 
export PATH=$PATH:/opt/omniORB-4.1.6/bin

# The different backends can be:
#BACKEND=yarp
#BACKEND=ros
BACKEND=standard
#BACKEND=extract

# Use, however, $BACKEND from local.mk
. ../local.mk

# Use a script argument
backend=${BACKEND,,}
echo "We will use backend \"$backend\""

EXAMPLE_DIR="../example/sum"
EXAMPLE_FILE="sum.idl"
EXAMPLE_RESULT="SumModule.h"

echo "./run.sh \"${EXAMPLE_DIR}/${EXAMPLE_FILE}\" > \"${EXAMPLE_DIR}/${EXAMPLE_RESULT}\""
./run.sh "${EXAMPLE_DIR}/${EXAMPLE_FILE}" "$backend" > "${EXAMPLE_DIR}/${EXAMPLE_RESULT}.new" 

no_lines=$(< "${EXAMPLE_DIR}/${EXAMPLE_RESULT}.new" wc -l)
diff_result=$(diff "${EXAMPLE_DIR}/${EXAMPLE_RESULT}" "${EXAMPLE_DIR}/${EXAMPLE_RESULT}.new")

if [[ $? -eq 0 ]]; then 
	echo "No differences of current generated header file with the previous one"
	rm ${EXAMPLE_DIR}/${EXAMPLE_RESULT}.new
else 
	if [[ $no_lines -eq 0 ]]; then
		echo "The generated header file is empty, do not use it. Probably omniidl is not installed."
		rm ${EXAMPLE_DIR}/${EXAMPLE_RESULT}.new
	else
		mv -f ${EXAMPLE_DIR}/${EXAMPLE_RESULT}.new ${EXAMPLE_DIR}/${EXAMPLE_RESULT}
	fi
fi

echo "Show piece of the (previously) generated header file:"
echo
cat -n "${EXAMPLE_DIR}/${EXAMPLE_RESULT}" | grep -v "//" | head -n 56 | tail -n 20
echo

# Compilation on server needs to be "ordered"
#   to be able to use this script, the rur-builder already has to be installed
#if [[ "$no_lines" < 10 ]]; then
#	echo "Error: no proper header file generated!"
#	exit 2
#fi

