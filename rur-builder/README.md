# RurBuilder
RurBuilder is a generator of (AI) modules or agents for different types of middlewares. It is developed by the cloud robotics company DO, shorthand for Distributed Organisms B.V.. The letter combination RUR stands for Roughly Universal Robot platform but is meant to be a group of modules in the cloud and over smartphones for any purpose. In robotic communities there are many robotic platforms available, such as ROS, YARP, OROCOS, MARIE, ACT-R, SOAR, IDA. The first few concentrating on providing middleware functionalities like connectors, ports, messages and a hardware abstraction layer, the latter focusing on (cognitive) control architectures, a dynamic set of functional units providing the robot with features like sensor fusion, attention, anticipation, and context-awareness.

The RUR platform aims to provide a means to independently write your software modules from any middleware. It does not prescribe how a cognitive architecture should be organized. Neither does it prescribe the communication infrastructure. It only requires you to write short interface description files for your modules. These are taken by the rur-builder to generate the glue code for the currently supported middleware solutions. These interface description files can also be used by crawlers on the web and be able to bootstrap distribution architectures for software modules (above the distribution frameworks for apps).

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

## Design Cycle with RUR
The common design cycle is like this:

- Write an .idl file for your component, can be a new or existing component. The component can contain several files, say we have "esn.cpp".
- Run the RUR compiler in the middleware-agnostic mode. This provides a language-specific header file "rur_esn.h".
- We make the "esn.cpp" file network-aware by making use of the port, channels, message structs defined in the "rur_esn.h" file.
- Run the RUR compiler in a middleware-specific mode. For example we run a yarp server on the robot. The "rur_esn.h" file will now be rewritten by yarp-specific function calls. Nothing needs to be changed in the "esn.cpp" file.
- Run the RUR compiler for another type of middleware, e.g. ROS. Now instead of the function calls for yarp, the header file will contain code for ros.

## Alternatives

Programming code is separated into different components. For example a "particle filter", an "echo state network", a "reinforcement learning module", or other type of components that provide algorithms required for robotic applications. Each component is described in a language-independent manner using a well-known variant of the CORBA IDL, interface description language. An IDL is normally used to have a language neural interface between components, such that these can be written in multiple languages. Specific instances of IDLs (besides the CORBA one) are Protocol Buffers (Google), Avro (Hadoop, Apache), Thrift (Facebook, Apache), and WSDL (tailored to web services).

The RUR platform provides the glue using the IDL specification of a component to generate a language-specific header file, which can be used to use this component in a certain middleware. So, from an IDL specification, code is generated that allows a component written in C to be used on a robot running the YARP middleware. However, using this same IDL specification a component written in Java can be used within the JADE multi-agent system. Of course, if the programmer allows a component to be used from within Java or C - by SWIG e.g. - this will make it possible to use the same component in either programming language, but this is not the task of the RUR platform.

The RUR platform is similar to a recent effort, Genom3, generator of modules. However, contrary to Genom3 which decorates existing code with generic keywords that will be replaced by middleware-specific terms, the component code will not be touched by the RUR compiler. This means that the code is still compilable, syntax highlighting still works properly, and declarations and references can be found by the indexer (in e.g. Eclipse).


## Installation

The rur-builder itself can be installed by:

    sudo make install

It does not need to be build, because it exists out of bash and Python scripts. To actually use the rur-builder it is highly recommend to install the AIM tools, follow for that the installation instructions at the ![the aim tools](https://github.com/mrquincle/aimtools "aimtools") repository.

## Example
An example that shows the first automatically generated structures, instances, and functions of a so-called ARTMAP module to be used in YARP middleware:

![alt text](https://github.com/mrquincle/rur-builder/raw/master/doc/rur_idl2yarp.jpg "IDL to YARP example")

You can find many examples in the ![the aim modules](https://github.com/mrquincle/aim_modules "aim modules") repository.

## Visualisation
The RurBuilder is integrated with the AIM GUI at https://github.com/mrquincle/ThreeNodes.js. This is a work in progress, and currently not the main focus.

## Copyrights
The copyrights (2012) belong to:

- Author: Anne van Rossum
- Almende B.V., http://www.almende.com and DO bots B.V., http://www.dobots.nl
- Rotterdam, The Netherlands
