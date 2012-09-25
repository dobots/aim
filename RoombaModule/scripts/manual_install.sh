#!/bin/bash

# Navigate to the parent directory of MADP, and execute "make" as well as "make install"
# now there are some files that have been forgotten to be installed.
# Make sure there is a "src" directory where you are, on my system the full path is:
#   SOURCE=/home/anne/myworkspace/roomba/MADPtrunk20120918/src
# and now execute ./manual_install.sh
#
# Copyright (c) Anne C. van Rossum
# Almende B.V. / DO bots
# License: LGPLv3
#

SOURCE=src

HEADER_PATH=/usr/local/include/madp
LIB_PATH=/usr/local/lib

sudo mkdir -p /usr/local/include/madp

sudo rsync -avzul ${SOURCE}/support/*.h ${HEADER_PATH}/support/
sudo rsync -avzul ${SOURCE}/planning/*.h ${HEADER_PATH}/planning/
sudo rsync -avzul ${SOURCE}/base/*.h ${HEADER_PATH}/base/
sudo rsync -avzul ${SOURCE}/include/*.h ${HEADER_PATH}/

sudo cp ${SOURCE}/libpomdp-solve/src/mdp/libmdp.a ${LIB_PATH}
sudo cp ${SOURCE}/libpomdp-solve/src/mdp/libmdp.a ${LIB_PATH}
