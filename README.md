# AIM

For a proper introduction see the [AIM website](http://dobots.github.io/aim/). This AIM repository currently contains:

* [rur-builder](https://github.com/dobots/rur-builder) to generate header files for a range of middlewares
* [aimtools](https://github.com/dobots/aimtools), a suite of tools to create, register, copy, and connect modules
* [zmqserver](https://github.com/dobots/zmqserver), a nameserver for ZeroMQ connections
* [zmqconnect](https://github.com/dobots/zmqconnect), a exogenous coordination method to connect two modules with each other
* [aimserver](https://github.com/dobots/aimserver), a REST interface for the aimtools

## For users

AIM is a framework to develop Artificial Intelligence software. This repository contains all tools that are necessary to make this work. A subset of the *functionality* of AI modules themselves can be found in [https://github.com/dobots/aim_center](https://github.com/dobots/aim_center). Every module requires a description file, so it can also be found easy on Google or Github, e.g.: [https://github.com/search/?q=aim-config+in:path+config.aim&type=Code](https://github.com/search/?q=aim-config+in:path+config.aim&type=Code).

In the end, the code will be available to you through a market place, called [Dodedodo](http:/www.dodedodo.com).

In order to download this repository, enter the following command:

    git clone --recursive https://github.com/dobots/aim.git

## For developers

You can best start with the description at [https://github.com/dobots/aimtools](https://github.com/dobots/aimtools) and create your first modules. 

## For AIM developers

The submodules do not update themselves, you will need to update this thing. Do not forget to use --recursive on cloning, or else the submodules will not be checked out. Afterwards, a series of commands will update this repository:

    git pull
    git submodule foreach git pull origin master
    git add -u
    git commit -m "Update $(date). Your message."
    git push

## Copyrights
The copyrights (2013) belong to:

- License: LGPL v.3
- Author: Anne van Rossum
- Almende B.V., http://www.almende.com and DO bots B.V., http://www.dobots.nl
- Rotterdam, The Netherlands

[![Analytics](https://ga-beacon.appspot.com/UA-46821459-1/dobots/aim)](https://github.com/dobots/aim)
