# RurBuilder
RurBuilder is a generator of (AI) modules or agents for different types of middlewares. 

## Middleware support
RurBuilder uses omniIDL and supports the following middlewares:

1. YARP, Yet Another Robot Platform
2. ROS, Robotic Operating System
3. ZeroMQ, ZeroMQ is the middleware used in the Storm data processing topology of Twitter
4. AgentScape (to be implemented)

## Abstraction support
The abstractions are written in IDL and look like a Java or C++ interface. The current abstractions allow for:

- incoming and outgoing ports, without the need to specify that they will be buffered ports in YARP, or publish/subscribe channels in ROS, etc. 
- automatic translation of custom structs to middleware-specific structs (preliminary)
- definition of input parameters on the command-line
- namespaces

## Usage
The RurBuilder can be used together with AIM (Artificial Intelligence Modules) utility tools:

- aimcreate-pkg: Creates a new module from template. This contains a "manifest.xml" file for use in ROS, a "CMakeLists.txt" file for use in YARP, a default Makefile, a few scripts to build and run using "default.mk" to retrieve the location of the RurBuilder and which backend to use.
- aimregister: Register new module. This extracts ports (input and output) and command-line parameters. It can be used by for example the AIM GUI to visualize the right number of ports with the proper names.
- aimrun: Run a module. The command-line parameters it expects must be part of the IDL description. One parameter that is required, is a "module_id" to be able to run multiple instances of the same AI module.
- aimstop: Stops a module. This can be done using the "module_id" defined at aimrun.
- aimconnect: Connects port/channel on one module to port on another module. The additional abstraction of ports/channels above only modules gives enough flexibility for most real-life situations. It is clear that the middleware only sets up how the AI modules are connected to each other, but does not dictate how the data is processed through this network of modules. This is left to the middleware.

Obviously, these tools are similar to the ones used in the ROS environment. Instead of creating ROS nodes, AI modules are created, etc.

## Installation

The rur-builder itself can be installed by:

    sudo make install

It does not need to be build, because it exists out of bash and Python scripts. To actually use the rur-builder it is highly recommend to install the AIM tools, follow for that the installation instructions at the ![the aim tools](https://github.com/mrquincle/aimtools "aimtools") repository.

## Example
An example that shows the first automatically generated structures, instances, and functions of a so-called ARTMAP module to be used in YARP middleware:

![alt text](https://github.com/mrquincle/rur-builder/raw/master/doc/rur_idl2yarp.jpg "IDL to YARP example")

## Visualisation
The RurBuilder is integrated with the AIM GUI at https://github.com/mrquincle/ThreeNodes.js

## Copyrights
The copyrights (2012) belong to:

- Author: Anne van Rossum
- Almende B.V., http://www.almende.com and DO bots B.V., http://www.dobots.nl
- Rotterdam, The Netherlands
