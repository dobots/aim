---
layout: default
title: Installation
---

# Installation

The actual installation and deployment might be daunting at first. However, most of the tools here are meant to help you develop faster, not to make things harder! We use standard tooling as much as possible. For example, github for code management, the omniORB compiler for IDL file parsing, etc.

## Example

At [AIM modules](https://github.com/dobots/aim_modules) you can find a ReadModule and a WriteModule that show how this framework operates. Both modules come with an IDL file (IDL stands for Interface Description Language). The [IDL file](https://raw.github.com/dobots/aim_modules/master/ReadModule/aim-config/ReadModule.idl) of the ReadModule does actually have only a few lines:

    #pragma copyright LGPLv3
    
    // Recommended namespace: rur
    module rur {
    
    // We want to be able to give our class a name
    interface ReadModule {
    
      // Input from sensor as individual values
      void Input(in long input);
    
    };
    }

The important part here is the function **void Input** defined with an additional keyword: **in**. This is transformed in a function description of which the following one shows the YARP-specific one:

    inline int *readInput(bool blocking=true) {
      Bottle *b = portInput->read(blocking);
      if (b != NULL) { 
        portInputValue = b->get(0).asInt();
        return &portInputValue;
      }
      return NULL;
    }

As you can see the generated code suddenly uses concepts that only mean something in YARP, such as an object called the **Bottle**. You do not need to know anything about these if you are just using the function readInput(), which is exactly what is meant by AIM allowing you to write middleware-agnostic code. The other parts of the code also appear in the generated header file (not shown). The \#pragma statements for example are transformed into comments and meant for author, copyright, date, and license information in the header. The **module** keyword translates into a namespace.

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
    

This will use the YARP middleware to set up a TCP connection between the WriteModule and the ReadModule to send over a value. The \"connect\" utility needs to know which middleware is used, on the moment it will require a server to be run on the machine, but this requirement can be lifted for a distributed environment, as long as the specific \"connect\" implementations of the middleware knows how to find the registered ports.

### Example with ZeroMQ

Running the code with ZeroMQ instead of YARP is along similar lines. To run the same WriteModule and ReadModule with ZeroMQ, you will need to:

    sudo apt-get install libzmq-dev
    sudo apt-get install libboost-dev libjson-spirit-dev
    

Older versions of boost json parser require a fix as mentioned on [stackoverflow](http://stackoverflow.com/questions/18552427/boost-read-json-and-c11).
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
    

Like you can see there is a almost no different with the YARP example, but now there is suddenly made use of ZeroMQ channels rather than YARP ones. The difference exists out of starting the right name server \"zmqserver\" rather than \"yarp server\", and run aimconnect with \"zeromq\" instead of \"yarp\".

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

So, how to go about to create your own module? Suppose we want to create a module that receives a value on an input port and sends a moving average on its output port. And we are planning on building our own home automation software, so we call our own little repository \"home\_automation\":

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
    

The output you can expect in the terminal with the MovingAverageModule (use Ctrl+Page Up/Down to navigate) is something like: \"Messages 
received: 4\" and \"Calculate (1+2\*3)/4=1.75\". Now upload your code to your own github repository!

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

