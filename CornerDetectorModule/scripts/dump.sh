#!/bin/bash


echo "Print first 64 bytes of \"$1\""
#cat $1 | head -n 2 | hexdump -v  -e '/1  "%_ad# "' -e '/1    "%02X hex"' -e '/1 " = %03i dec"' -e '/1 " = %03o oct"' -e '/1 " = _%c\_\n"'

cat $1 | head -c 72 | hexdump -v -e '/8 "%04_ad: "' -e '/1 "%02X "' -e '/8 " | "' -e '/1 "%03u "' -e '/8 "\n"'

echo
