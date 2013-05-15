#!/bin/bash

cd ../bin

SCRIPTPATH=../scripts

./camserver /dev/video0

echo "We are at " `pwd`
echo "Run ${SCRIPTPATH}/dump.sh result.bmp"
${SCRIPTPATH}/dump.sh result.bmp
${SCRIPTPATH}/dump.sh ../data/replicator_grayscale.bmp
${SCRIPTPATH}/dump.sh ../data/replicator.bmp
