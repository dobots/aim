# RurBuilder
RurBuilder is a generator of (AI) modules or agents for different types of middlewares. 

## Middleware support
RurBuilder uses omniIDL and supports the following middlewares:
1. YARP, Yet Another Robot Platform
2. ROS, Robotic Operating System
3. AgentScape (to be implemented)

## Abstraction support
The abstractions are written in IDL and look like a Java or C++ interface. The current abstractions allow for:
- incoming and outgoing ports, without the need to specify that they will be buffered ports in YARP, or publish/subscribe channels in ROS, etc. 
- automatic translation of custom structs to middleware-specific structs (preliminary)
- definition of input parameters on the command-line
- namespaces

## Usage
The RurBuilder can be used together with AIM (Artificial Intelligence Modules) utility tools:
- aimcreate-pkg Creates a new module from template
- aimregister Register new module (extracts ports and command-line parameters) 
- aimrun Run a module
- aimstop Stops a module
- aimconnect Connects port/channel on one module to port on another module

Obviously, these tools are similar to the ones used in the ROS environment. Instead of creating ROS nodes, AI modules are created, etc.

## Example
An example that shows the first automatically generated structures, instances, and functions of a so-called ARTMAP module to be used in YARP middleware:
![alt text](https://github.com/mrquincle/rur-builder/raw/master/doc/rur_idl2yarp.jpg "IDL to YARP example")

## Visualisation
The RurBuilder is integrated with the AIM GUI at https://github.com/mrquincle/ThreeNodes.js

## Copyrights
The copyrights (2012) belong to:
  Author: Anne van Rossum
  Almende B.V. and DO bots B.V.
  Rotterdam, The Netherlands
