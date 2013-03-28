// general C/C++ headers
#include <vector>
#include <iostream>
#include <string>

// middleware specific headers
#include <ros/ros.h>
#include "std_msgs/String.h"

// namespaces and typedefs
using namespace ros;

// recommended namespace: "Rur"
// add "using namespace Rur;" to your .cpp file
namespace rur {

struct Param {
  string module_id;
  int parameter;
}

// The generated class. Do not modify or add class members
// Either derive from this class and implement Tick() or
// use a separate helper class to store state information.
// All information for the operation of the module should 
// be obtained over the defined ports
class MovingAverageModule {
private:
  NodeHandle n;
  
  std::vector< int> portInputValues;
  std::vector< Subscriber <std_msgs::Int16>* > portInput;
  
  std::vector< Publisher <std_msgs::Int16>* > portAverage;

public:
  // The constructor needs to be called, also when you derive from this class
  MovingAverageModule() {
    
    for (int i = 0; i < 1; ++i) {
      portInputValues.push_back(-1);
      string portName = "/input" + string(i);
      portInput.push_back(n.subscribe (portName.c_str(), 1000, boost::bind(&MovingAverageModule::InputCallback, this, _1, i) ) );
    }
    
    for (int i = 0; i < 1; ++i) {
      string portName = "/average" + string(i);
      portAverage.push_back(n.advertise <std_msgs::Int16>(portName.c_str(), 1000) );
    }
  }
  
  ~MovingAverageModule() { }
  
  // This is the function you will need to implement.
  void Tick(); 
  
  // After construction you will need to call this function first
  void Init() {
    int argc = 1;
    char** argv = NULL;
    ros::init(argc, argv, "movingaveragemodule");
  }
  
protected:
  // All subsequent functions should be called from "within" this module
  // From either the Tick() routine itself, or Tick() in a derived class
  
  inline int *readInput(int index = 0) {
    return &portInputValues[index];
  }
  
  void InputCallback(std_msgs::Int16 & msg, int index) {
    portInputValues[index] = msg;
  }
  
  inline void writeAverage(const int output, int index = 0) {
    std_msgs::Int16 msg = output;
    portAverage[index]->publish(msg);
  }
  
};
}

