// general C/C++ headers
#include <vector>
#include <iostream>
#include <string>

// middleware specific headers
#include <ros/ros.h>
#include "std_msgs/String.h"
#include "std_msgs/Int16.h"


// recommended namespace: "Rur"
// add "using namespace Rur;" to your .cpp file
namespace rur {

// The generated class. Do not modify or add class members
// Either derive from this class and implement Tick() or
// use a separate helper class to store state information.
// All information for the operation of the module should 
// be obtained over the defined ports
class WriteModule {
private:
  ros::NodeHandle n;
  std::string module_id;
  
  ros::Publisher portOutput;

public:
  // The constructor needs to be called, also when you derive from this class
  WriteModule() {
    
    {
      std::string portName = "/output" + module_id;
      portOutput = n.advertise <std_msgs::Int16>(portName.c_str(), 1000);
    }
  }
  
  ~WriteModule() { }
  
  // This is the function you will need to implement.
  void Tick() {}
  
  // After construction you will need to call this function first
  void Init(std::string module_id) {
    this->module_id = module_id;
    int argc = 1;
    char** argv = NULL;
    ros::init(argc, argv, "writemodule");
  }
  
protected:
  // All subsequent functions should be called from "within" this module
  // From either the Tick() routine itself, or Tick() in a derived class
  
  inline bool writeOutput(const int output) {
    std_msgs::Int16 msg; msg.data = output;
    portOutput.publish(msg);
    return true;
  }
  
};
}

