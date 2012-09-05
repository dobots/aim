#!/bin/bash

cd ../build

DEBUG="gdb --args"
#DEBUG=
module_id=2
bluetooth_address=00:06:66:45:B7:46
$DEBUG ./BlueSourceModule $module_id $bluetooth_address
