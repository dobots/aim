#!/bin/bash

echo "We make the example \"sum\"" 
export PATH=$PATH:/opt/omniORB-4.1.6/bin

. ../local.mk

# The different backends can be:
#BACKEND=yarp
#BACKEND=ros
BACKEND=standard
#BACKEND=extract

# Use a script argument
backend=${BACKEND,,}
echo "We will use backend \"$backend\""

EXAMPLE_DIR="../example/sum"
EXAMPLE_FILE="sum.idl"
EXAMPLE_RESULT="SumModule.h"

echo "./run.sh \"${EXAMPLE_DIR}/${EXAMPLE_FILE}\" > \"${EXAMPLE_DIR}/${EXAMPLE_RESULT}\""
./run.sh "${EXAMPLE_DIR}/${EXAMPLE_FILE}" "$backend" > "${EXAMPLE_DIR}/${EXAMPLE_RESULT}" 

#cat "${EXAMPLE_DIR}/${EXAMPLE_RESULT}"
