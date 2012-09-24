#!/bin/bash

sudo mkdir -p /usr/local/include/madp

sudo rsync -avzul src/support/*.h /usr/local/include/madp/support/
sudo rsync -avzul src/planning/*.h /usr/local/include/madp/planning/
sudo rsync -avzul src/base/*.h /usr/local/include/madp/base/
sudo rsync -avzul src/include/*.h /usr/local/include/madp/
