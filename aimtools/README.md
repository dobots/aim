<!-- Uses markdown syntax for neat display at github -->

# AIM Tools
AIM stands for Artificial Intelligence Modules

## What does it do?
The AIM tools are meant for management of separate modules that each have AI functionality. They can be compared with the utilities that are provided with ROS, such as roscreate-pkg etc (of which I was not aware), or with nodejs, such as npm (of which I was neither aware). The tools work together with [rur-builder](https://github.com/mrquincle/rur-builder), a (python) backend for omniidl. 

The current AIM tools are tailored to [YARP](http://eris.liralab.it/yarp/) and generate code that makes it easy to work with YARP as middleware. However, nothing prevents further extensions to make it easier to wrap code as ROS modules or something else, even Java. It depends on the backends provided by the forementioned rur-builder.

## Is it good?
The maturity of this software can be improved. However, it has been used already in quite some different scenarios. One of them is to connect to the servers of an Almende spinoff in sensor data cloud services ([Sense Observation Systems](http://sense-os.nl), another use case has been the [Surveyor](http://www.surveyor.com/SRV_info.html) robots.

## What are the alternatives?
There are no known alternatives. Most modular approaches tie the user to a certain middleware. 

## How to install?
The AIM tools work together with the [rur-builder](https://github.com/mrquincle/rur-builder). To install:

* sudo apt-get install omniidl # the only dependency
* cd /opt
* git clone https://github.com/mrquincle/rur-builder.git
* git clone https://github.com/mrquincle/aimtools.git
* cd aimtools
* cd aimcreate-pkg
* make; sudo make install # on the question fill in the proper rur-builder/backends directory
* cd "your workspace"
* aimcreate-pkg YourModule # convention: all AIM binaries end with "Module"

## How to install YARP?
The YARP middleware is optionally. To install on Ubuntu:

* sudo add-apt-repository ppa:yarpers/yarp
* sudo apt-get update
* sudo apt-cache search yarp
* sudo apt-get install libyarp yarp-bin # and perhaps yarp-view

## Example
Now you will have a module created for you. You should be able to run "make" directly in it. This will use the default .idl file and generate the proper "YourModule.h" header file in the "aim" directory.

Suppose this is the parameter you want to send over the commandline to the module, then you add this to the YourModule.idl file:

	struct Param {
		string filename;
	};

Subsequently in your YourModuleMain.cpp file you will need to set it:

	int main(int argc, char *argv[])  {
		YourModule *m = new YourModule();
		Param * param = m->GetParam();
		...
		param->filename = argv[2]; // let's say it's the second argument
		...
	}

Then, for an incoming channel, you write this in the YourModule.idl file:

	interface WriteToFileModule {
		void Input(in long data);
	};

This will automatically generate a "/data" channel for you in the case YARP is chosen as backend (more specifically, it will be /yourmodule{id}/data). The only thing you will need to do now is to write functional code that uses the channels in the stub file YourModule.cpp, for example:

	void WriteToFileModule::Tick() {
		double input = *readInput();
		ofstream myfile(cliParam->filename.c_str(), ios::app);
		myfile << input << '\n';
		myfile.close();
	}

As you can see the "Input" function in the YourModule.idl file has become something you can read from and which returns the proper type. All conversions necessary are done in the automatically generated header file (for example YARP does use Bottle's to communicate such datatypes, and you don't need to know anything about that using this framework).

Note: this example illustrates that a filestream is created and closed each "Tick()". To remedy this you should not add anything to the header file because that file should be automatically be regenerated as soon as you change the idl description of the module. Hence, the recommended method is to subclass YourModule to add state information to it. And you have subsequently to adjust the YourModuleMain.cpp file to call this subclass. If you have already an existing important class, this is also an easy way to integrate this middleware-agnostic layer. Just add also YourModule as a parent to that class.

## Where can I read more?
* [Wikipedia (YARP)](http://en.wikipedia.org/wiki/YARP)

## Copyrights
The copyrights (2012) belong to:

- Author: Anne van Rossum
- Author: Scott Guo
- Almende B.V., http://www.almende.com and DO bots B.V., http://www.dobots.nl
- Rotterdam, The Netherlands
