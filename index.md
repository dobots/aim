---
layout: default
title: Introduction
---

# Introduction

Artificial Intelligence Modules

[View AIM on GitHub](https://github.com/dobots/aim-bzr)

## Welcome to AIM

AIM stands for Artificial Intelligence Modules, but is much more than that. AIM is inspired by three recent movements in practical computer science. First, in the robotics community there is a need for a wide range of functionalities, from computer vision, navigation and mapping, to sensor fusion, and machine learning, which lead to several **middlewares** that allow people to collaborate: [YARP](http://eris.liralab.it/yarp) (Yet Another Robot Platform), [ROS](http://www.ros.org) (Robot Operating System), [URBI](http://www.urbiforge.org) (Universal Real-time Behavior Interface) to name a few. Second, there are **market** infrastructures to distribute your applications: [Google play](https://play.google.com), [Apple app store](http://www.apple.com/iphone/from-the-app-store/), and repositories like the [Launchpad package archives](https://launchpad.net/~dobots/+archive/ppa) and the [openSUSE build service](https://build.opensuse.org/). Third, there are frameworks to allow you to implement or reveal functionality in the **cloud**, from [ZeroMQ](http://www.zeromq.org/), a distributed transport layer used in Twitter [Storm](http://storm-project.net/), to [Mahout](http://mahout.apache.org/), scalable Map/Reduce based machine learning libraries.

## Goal

The goal of AIM is threefold:

* A **middleware-agnostic** modular approach to writing code: the **rur-builder**.
* Tools to easily **create, publish, and cross-compile modules** that run as smoothly on robots as on servers: the **aimtools**.
* A **distributed market place**, an app store for web developers: the **aimserver**.

## Background

The **rur-builder** is loosely similar to [Genom3](http://homepages.laas.fr/mallet/soft/architecture/genom3). Just as Genom3 it is aimed to create an infrastructure of blocks of software that somehow know how to talk to each other. Genom3 and the rur-builder does not create some additional library that has to be linked to all middlewares (YARP, ROS, URBI, AgentScape, ZeroMQ, NodeJS) which quickly becomes a pain. In contrary, Genom3 and the rur-builder perform code generation. However, different from Genom3 using the rur-builder does not actually change the code itself. We do not "enrich" the code with meta-information. The meta-information is expected to be written down in an **IDL file**. This IDL file describes the interface of a module in a general sense. This means for example (contrary to Genom3) that code completion still works: there are no meta-statements (in TCL) through the code. This has multiple advantages. a.) The indexer just works. b.) A standard backend can be used for prototyping, not requiring any middleware. c.) Code generation in the headers can be extremely flexible, e.g. implementing topics for ROS, a concept totally absent in YARP. This is hard to achieve this flexibility by meta-statements in the code itself.

## Components

The current rur-builder provides functionality for different middlewares. Currently supported are **YARP**, **ROS**, **ZeroMQ**, **Android**, and **NodeJS** as middleware between the different AI modules. To be able to use ZeroMQ as middleware solution a nameserver is implemented, called "zmqserver", and "zmqconnect", a connect utility (allowing exogenous coordination: start two modules separately and only later on connect them together). When using NodeJS as middleware, the module will built as a NodeJS library. The Android middleware will create an app, where the communication makes use of Android's messenger class and relies on the Dodedodo app as nameserver.

The **aimtools** is a suite of tools in the same spirit as some of the tools in ROS, such as roscreate-pkg:

* **aimbrowse**: search for repositories with AIM modules
* **aimget**: add a github repository to your AIM workspace
* **aiminit**: create your own github repository in your AIM workspace
* **aimcreate-pkg**: create a new AIM module
* **aimcreate-relay-pkg**: create a new module to merge, split, or convert ports
* **aimcopy**: copies a module
* **aimupdate**: update a module to a newer version of AIM
* **aimregister**: register the module locally
* **aimselect**: select a specific middleware to compile for
* **aimmake**: compile the module
* **aimcross**: cross-compile the module (currently supported: blackfin; upcoming: raspberry)
* **aimrun**: run a previously registered module
* **aimconnect**: connect two ports on running modules
* **aimstop**: stop the module
* **aimdebug**: create a file with useful info for an issue report

The **aimserver** (forthcoming) is a REST server that allows people to find AI modules across a wide range of repositories. Moreover, it provides functionality to compile the code found, run it, stop it, and connect modules together. Towards that purpose there is a range of tools to create, register, copy, and connect AI modules: "aimtools".

## Example

At [AIM modules](https://github.com/dobots/aim_modules) you can find a ReadModule and a WriteModule that show how this framework operates. Both modules come with an IDL file (IDL stands for Interface Description Language). The [IDL file](https://raw.github.com/dobots/aim_modules/master/ReadModule/aim/idl/ReadModule.idl) of the ReadModule does actually have only a few lines:

    #pragma copyright LGPLv3
    
    // Recommended namespace "rur"
    module rur {
    
    // We want to be able to give our class a name
    interface ReadModule {
    
      // Input from sensor as individual values
      void Input(in long input);
    
    };
    }

The important part here is the function "void Input" defined with an additional keyword: "in". This is transformed in a function description of which the following one shows the YARP-specific one:

    inline int *readInput(bool blocking=true) {
      Bottle *b = portInput->read(blocking);
      if (b != NULL) { 
        portInputValue = b->get(0).asInt();
        return &portInputValue;
      }
      return NULL;
    }

As you can see the generated code suddenly uses concepts that only mean something in YARP, such as an object called the "Bottle". You do not need to know anything about these if you are just using the function readInput(), which is exactly what is meant by AIM allowing you to write middleware-agnostic code. The other parts of the code also appear in the generated header file (not shown). The \#pragma statements for example are transformed into comments and meant for author, copyright, date, and license information in the header. The "module" keyword translates into a namespace.

## Installation
The installation boils down to make and sudo make install after cloning the github repositories, and setting an environmental variable.
First install OmniIdl, required by the rur-builder:

    sudo apt-get install omniidl # on Ubuntu
    brew install omniorb # on Mac OS X

Then install the AIM tools:

    git clone --recursive https://github.com/dobots/aim.git
    cd aim
    make aimtools rur-builder
    sudo make aimtools.install rur-builder.install
    mkdir $HOME/aim_workspace # Or some other dir
    export AIM_WORKSPACE=$HOME/aim_workspace
    echo "export AIM_WORKSPACE=$AIM_WORKSPACE" >> $HOME/.bashrc 
    

Now to get some modules, we need to clone a module repository. Towards this purpose we use aimget:

    aimget examples https://github.com/dobots/aim_modules
    cd $AIM_WORKSPACE/examples
    aimmake ReadModule
    aimregister ReadModule
    aimmake WriteModule
    aimregister WriteModule
    

Now you have downloaded a bunch of modules that you subsequently can compile and run in different ways.

### Example with YARP

The running of the code of these modules actually depends on the middleware you want to use it with. 
So [install YARP](http://wiki.icub.org/yarpdoc/install.html) first.
Let us assume you downloaded the robotics AI modules as mentioned above then:

    cd $AIM_WORKSPACE/examples
    aimselect ReadModule yarp
    aimmake ReadModule
    aimregister ReadModule
    aimselect WriteModule yarp
    aimmake WriteModule
    aimregister WriteModule
    # go to a separate shell (Ctrl+Shift T in gnome-terminal)
    yarp server
    # go to a separate shell (Ctrl+Shift T in gnome-terminal)
    aimrun ReadModule 0 
    # go to a separate shell (Ctrl+Shift T in gnome-terminal)
    aimrun WriteModule 0
    # go to a separate shell (Ctrl+Shift T in gnome-terminal)
    aimconnect yarp WriteModule 0 output ReadModule 0 input
    

This will use the YARP middleware to set up a TCP connection between the WriteModule and the ReadModule to send over a value. The "connect" utility needs to know which middleware is used, on the moment it will require a server to be run on the machine, but this requirement can be lifted for a distributed environment, as long as the specific "connect" implementations of the middleware knows how to find the registered ports.

### Example with ZeroMQ

Running the code with ZeroMQ instead of YARP is along similar lines. To run the same WriteModule and ReadModule with ZeroMQ, you will need to:

    sudo apt-get install libzmq-dev
    sudo apt-get install libboost-dev libjson-spirit-dev
    

Older versions of boost json parser require a fix as mentioned on [stackoverflow.](http://stackoverflow.com/questions/18552427/boost-read-json-and-c11)
Install the zmqserver, this requires [Node.js to be installed](https://github.com/joyent/node).

    cd aim # From the dir where you cloned aim.git in
    make zmqconnect zmqserver
    sudo make zmqconnect.install zmqserver.install
    

    cd $AIM_WORKSPACE/examples
    cd ReadModule
    aimselect zeromq
    aimmake
    aimregister # if not done before
    cd ../WriteModule
    aimselect zeromq
    aimmake
    aimregister # if not done before
    # go to a separate shell (Ctrl+Shift T in gnome-terminal)
    zmqserver
    # go to a separate shell (Ctrl+Shift T in gnome-terminal)
    aimrun ReadModule 0 
    # go to a separate shell (Ctrl+Shift T in gnome-terminal)
    aimrun WriteModule 0
    # go to a separate shell (Ctrl+Shift T in gnome-terminal)
    aimconnect zeromq WriteModule 0 output ReadModule 0 input
    

Like you can see there is a almost no different with the YARP example, but now there is suddenly made use of ZeroMQ channels rather than YARP ones. The difference exists out of starting the right name server "zmqserver" rather than "yarp server", and run aimconnect with "zeromq" instead of "yarp".

## Create your own module

Before you create your own module, which is automatically generated below, we want to set some information about yourself. If you do not fill in this information, Homer Simpson will be the one credited for your code. The file that will be queried for this is:

    $HOME/.rur/personal_info.txt

The type of information you have to fill in (you can always change this later and update your files manually, most of it ends up in comment sections in files and in description files for example Android builds):

    yourname="Homer J. Simpson"
    youremail="chunkylover53@aol.com"
    yourorganisation="Springfield Power Company"
    yourproject="Project Bootstrap"
    license="State"
    license_abbreviation="TR8OR"

So, how to go about to create your own module? Suppose we want to create a module that receives a value on an input port and sends a moving average on its output port. And we are planning on building our own home automation software, so we call our own little repository "home\_automation":

    aiminit home_automation
    cd $AIM_WORKSPACE/home_automation
    aimcreate-pkg MovingAverageModule
    # Now we compile our module, the default module already compiles!
    aimmake MovingAverageModule

We are now going to make our changes to the code at three places. Open MovingAverageModule/aim-config/MovingAverageModule.idl with your favorite editor and make sure the interface definition looks like the following:

    interface MovingAverageModule {
      void Input(in long input);
      
      void Average(out long output);
    }

Adjust MovingAverageModule/inc/MovingAverageModuleExt.h to just add two private fields to the class (to count the messages, and to store the last average):

    class MovingAverageModuleExt: public MovingAverageModule {
    public:
      MovingAverageModuleExt(): message_counter(0), prev_average(0) { }
      virtual ~MovingAverageModuleExt() { }
      void Tick();
      bool Stop();
    private:
      long int message_counter;
      double prev_average;
    };
    

Adapt MovingAverageModule/src/MovingAverageModuleExt.cpp subsequently:

    void MovingAverageModuleExt::Tick() {
      int *value = readInput();
      if (value) {
        int i = ++message_counter;
        std::cout << "Messages received: " << i << std::endl;
        double average = (*value + prev_average*(i-1)) / i;
        std::cout << "Calculate (" << *value << "+" << prev_average << "*" << (i-1) << ") /" << i << "=" << average << std::endl;
        writeAverage(average);
        prev_average = average;
      }
      sleep(1); // Requires to #include unistd.h
    }

The complete code you can find at the [MovingAverageModule](https://github.com/mrquincle/aim_modules/tree/master/MovingAverageModule) on github. Running your example is in the same way as defined with the ReadModule and WriteModule example. Let us use the ZeroMQ backend (and assume that you followed the example above with the ReadModule and the WriteModule currently compiled for the ZeroMQ backend indeed):

    cd $AIM_WORKSPACE/home_automation
    aimselect MovingAverageModule zeromq
    aimmake MovingAverageModule
    aimregister MovingAverageModule
    # go to a separate shell (Ctrl+Shift T in gnome-terminal)
    zmqserver
    # go to a separate shell (Ctrl+Shift T in gnome-terminal)
    aimrun ReadModule 0 
    # go to a separate shell (Ctrl+Shift T in gnome-terminal)
    aimrun WriteModule 0
    # go to a separate shell (Ctrl+Shift T in gnome-terminal)
    aimrun MovingAverageModule 0
    # go to a separate shell (Ctrl+Shift T in gnome-terminal)
    aimconnect zeromq MovingAverageModule 0 average ReadModule 0 input
    aimconnect zeromq WriteModule 0 output MovingAverageModule 0 input
    

The output you can expect in the terminal with the MovingAverageModule (use Ctrl+Page Up/Down to navigate) is something like: "Messages 
received: 4" and "Calculate (1+2\*3)/4=1.75". Let's upload your code to your own github repository!

    cd $AIM_WORKSPACE/home_automation
    git add MovingAverageModule
    git commit -m "A moving average module for the AIM repository"
    git push
    # And now you can push it to e.g. github 
    git remote add origin git@github.com:yourname/somerepos.git
    git push -u origin master
    

## How to get it?

You can get the code from [github.com](https://github.com/dobots/aim), but the easiest way to get the code is via [Launchpad](https://launchpad.net/~dobots/+archive/ppa):

    sudo add-apt-repository ppa:dobots/ppa 
    sudo apt-get update
    sudo apt-get install aim
    dpkg -L aim

## Example of the tree layout of MovingAverageModule

To be completely transparent about how a module is organized, this is the complete listing of an AIM module except for the files generated in its builds/ directory. This listing is generated by: 

    tree -H MovingAverageModule

Summarized, the aim-core files do not need modifications, the aim-config files will need so. By default they already contain templated text, which will make them compile. However, if you for example want to publish your module to be used by the Node package manager you might want to add some additional description in package.json for instance. We will automate this process more and more so you will need fewer places to do this over time.

    [MovingAverageModule](MovingAverageModule)  
    ├── [aim-config](./aim-config/)  
    │   ├── [config.aim](./aim-config/config.aim)  
    │   ├── [MovingAverageModule.idl](./aim-config/MovingAverageModule.idl)  
    │   ├── [nodejs](./aim-config/nodejs/)  
    │   │   ├── [binding.gyp](./aim-config/nodejs/binding.gyp)  
    │   │   ├── [MovingAverageModuleNode.cc](./aim-config/nodejs/MovingAverageModuleNode.cc)  
    │   │   └── [package.json](./aim-config/nodejs/package.json)  
    │   └── [ros](./aim-config/ros/)  
    │       ├── [manifest.xml](./aim-config/ros/manifest.xml)  
    │       └── [package.xml](./aim-config/ros/package.xml)  
    ├── [aim-core](./aim-core/)  
    │   ├── [cmake](./aim-core/cmake/)  
    │   │   ├── [FindJsonSpirit.cmake](./aim-core/cmake/FindJsonSpirit.cmake)  
    │   │   ├── [FindYARP.cmake](./aim-core/cmake/FindYARP.cmake)  
    │   │   ├── [FindZMQ.cmake](./aim-core/cmake/FindZMQ.cmake)  
    │   │   ├── [LibFindMacros.cmake](./aim-core/cmake/LibFindMacros.cmake)  
    │   │   └── [MacroEnsureOutOfSourceBuild.cmake](./aim-core/cmake/MacroEnsureOutOfSourceBuild.cmake)  
    │   ├── [CMakeLists.txt](./aim-core/CMakeLists.txt)  
    │   ├── [config.h.cmake](./aim-core/config.h.cmake)  
    │   ├── [inc](./aim-core/inc/)  
    │   │   └── [MovingAverageModule.h](./aim-core/inc/MovingAverageModule.h)  
    │   ├── [middleware\_default.aim](./aim-core/middleware_default.aim)  
    │   ├── [middleware\_name.sh](./aim-core/middleware_name.sh)  
    │   └── [src](./aim-core/src/)  
    │       └── [MovingAverageModule.cpp](./aim-core/src/MovingAverageModule.cpp)  
    ├── [aim-devel](./aim-devel/)  
    ├── [binding.gyp](./binding.gyp)  
    ├── [CMakeLists.txt](./CMakeLists.txt)  
    ├── [inc](./inc/)  
    │   └── [MovingAverageModuleExt.h](./inc/MovingAverageModuleExt.h)  
    ├── [Makefile](./Makefile)  
    ├── [server.js](./server.js)  
    └── [src](./src/)  
    ├── [MovingAverageModuleExt.cpp](./src/MovingAverageModuleExt.cpp)  
    └── [MovingAverageModuleMain.cpp](./src/MovingAverageModuleMain.cpp)  


## Support {#support}

You would like to know more? Check out the company providing all this open-source software [Distributed Organisms B.V.](http://www.dobots.nl), a spinoff of the Dutch research institute [Almende](http://www.almende.com). The github repository of DO is called [dobots](https://github.com/dobots). You can also checkout the repositories of individual authors [Bart van Vliet](https://github.com/vliedel), [Dominik Egger](https://github.com/eggerdo) and [Anne van Rossum](https://github.com/mrquincle).

[![](https://secure.gravatar.com/avatar/36365a1f813c2fe401024f5d3552adaf?s=30&d=https://a248.e.akamai.net/assets.github.com%2Fimages%2Fgravatars%2Fgravatar-user-420.png)](https://github.com/mrquincle) [mrquincle](https://github.com/mrquincle) maintains [AIM](https://github.com/mrquincle/aim-bzr)

## Open source {#opensource}

Our software is offered as open-source software under LGPLv3 or Apache license. Distributed Organisms B.V. (or Dobots) is an Almende spin-off. This group uses this software in all kind of commercial products, which allows us to provide it for free to the community at large.

