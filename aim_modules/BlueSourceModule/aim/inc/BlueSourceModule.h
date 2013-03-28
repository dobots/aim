/**
 * This file is created at Almende B.V. It is open-source software and part of the Common 
 * Hybrid Agent Platform (CHAP). A toolbox with a lot of open-source tools, ranging from 
 * thread pools and TCP/IP components to control architectures and learning algorithms. 
 * This software is published under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless,
 * we personally strongly object against this software being used by the military, in the
 * bio-industry, for animal experimentation, or anything that violates the Universal
 * Declaration of Human Rights.
 *
 * Copyright (c) 2010 Anne van Rossum <anne@almende.com>
 *
 * @author  Anne C. van Rossum
 * @company Almende B.V.
 */

// general C/C++ headers
#include <vector>
#include <string>
#include <sstream>

// middleware specific headers
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Bottle.h>

// namespaces and typedefs
using namespace yarp::os;

// recommended namespace: "rur"
// do not forget to add "using namespace rur;" to your .cpp file
namespace rur {

struct Param {
  std::string module_id;
  int sensor_id;
};

// The generated class. Do not modify or add class members
// Either derive from this class and implement Tick() or
// use a separate helper class to store state information.
// All information for the operation of the module should 
// be obtained over the defined ports
class BlueSourceModule {
private:
  Network yarp;
  std::string module_id;
  
  // the port portMicrophone itself
  BufferedPort<Bottle> *portMicrophone;
  
  // the port portBattery itself
  BufferedPort<Bottle> *portBattery;
  
  // the port portLight itself
  BufferedPort<Bottle> *portLight;
  
  // the port portInfrared itself
  BufferedPort<Bottle> *portInfrared;
  
  // the port portMotorCurrent1 itself
  BufferedPort<Bottle> *portMotorCurrent1;
  
  // the port portMotorCurrent2 itself
  BufferedPort<Bottle> *portMotorCurrent2;
  
  // the port portWheel1 itself
  BufferedPort<Bottle> *portWheel1;
  
  // the port portWheel2 itself
  BufferedPort<Bottle> *portWheel2;
  
  // the port portLed1 itself
  BufferedPort<Bottle> *portLed1;
  
  // the port portLed2 itself
  BufferedPort<Bottle> *portLed2;
  
  // the port portLed3 itself
  BufferedPort<Bottle> *portLed3;
  
  // User-defined structs (automatically allocated later)
  Param *cliParam;

public:
  // The constructor needs to be called, also when you derive from this class
  BlueSourceModule() {
    cliParam = new Param();
    portMicrophone = new BufferedPort<Bottle>();
    portBattery = new BufferedPort<Bottle>();
    portLight = new BufferedPort<Bottle>();
    portInfrared = new BufferedPort<Bottle>();
    portMotorCurrent1 = new BufferedPort<Bottle>();
    portMotorCurrent2 = new BufferedPort<Bottle>();
    portWheel1 = new BufferedPort<Bottle>();
    portWheel2 = new BufferedPort<Bottle>();
    portLed1 = new BufferedPort<Bottle>();
    portLed2 = new BufferedPort<Bottle>();
    portLed3 = new BufferedPort<Bottle>();
  }
  
  ~BlueSourceModule() {
    delete portMicrophone;
    delete portBattery;
    delete portLight;
    delete portInfrared;
    delete portMotorCurrent1;
    delete portMotorCurrent2;
    delete portWheel1;
    delete portWheel2;
    delete portLed1;
    delete portLed2;
    delete portLed3;
    delete cliParam;
  }
  
  // This is the function you will need to override in a subclass implement.
  void Tick() {} 
  
  bool Stop() { return false; } 
  
  
  // After construction you will need to call this function first
  // it opens the YARP ports
  void Init(std::string module_id) {
    this->module_id = module_id;
    
    {
      std::stringstream portName; portName.str(); portName.clear();
      portName << "/bluesourcemodule" << module_id << "/microphone";
      portMicrophone->open(portName.str().c_str());
    }
    {
      std::stringstream portName; portName.str(); portName.clear();
      portName << "/bluesourcemodule" << module_id << "/battery";
      portBattery->open(portName.str().c_str());
    }
    {
      std::stringstream portName; portName.str(); portName.clear();
      portName << "/bluesourcemodule" << module_id << "/light";
      portLight->open(portName.str().c_str());
    }
    {
      std::stringstream portName; portName.str(); portName.clear();
      portName << "/bluesourcemodule" << module_id << "/infrared";
      portInfrared->open(portName.str().c_str());
    }
    {
      std::stringstream portName; portName.str(); portName.clear();
      portName << "/bluesourcemodule" << module_id << "/motorcurrent1";
      portMotorCurrent1->open(portName.str().c_str());
    }
    {
      std::stringstream portName; portName.str(); portName.clear();
      portName << "/bluesourcemodule" << module_id << "/motorcurrent2";
      portMotorCurrent2->open(portName.str().c_str());
    }
    {
      std::stringstream portName; portName.str(); portName.clear();
      portName << "/bluesourcemodule" << module_id << "/wheel1";
      portWheel1->open(portName.str().c_str());
    }
    {
      std::stringstream portName; portName.str(); portName.clear();
      portName << "/bluesourcemodule" << module_id << "/wheel2";
      portWheel2->open(portName.str().c_str());
    }
    {
      std::stringstream portName; portName.str(); portName.clear();
      portName << "/bluesourcemodule" << module_id << "/led1";
      portLed1->open(portName.str().c_str());
    }
    {
      std::stringstream portName; portName.str(); portName.clear();
      portName << "/bluesourcemodule" << module_id << "/led2";
      portLed2->open(portName.str().c_str());
    }
    {
      std::stringstream portName; portName.str(); portName.clear();
      portName << "/bluesourcemodule" << module_id << "/led3";
      portLed3->open(portName.str().c_str());
    }
  }
  
  // Before destruction you will need to call this function first
  // it closes the YARP ports
  void Close() {
    portMicrophone->close();
    portBattery->close();
    portLight->close();
    portInfrared->close();
    portMotorCurrent1->close();
    portMotorCurrent2->close();
    portWheel1->close();
    portWheel2->close();
    portLed1->close();
    portLed2->close();
    portLed3->close();
  }
  
  // Function to get Param struct (to subsequently set CLI parameters)
  inline Param *GetParam() { return cliParam; };
  
protected:
  // All subsequent functions should be called from "within" this module
  // From either the Tick() routine itself, or Tick() in a derived class
  
  inline bool writeMicrophone(const int microphone) {
    Bottle &microphonePrepare = portMicrophone->prepare();
    microphonePrepare.clear();
    microphonePrepare.addInt(microphone);
    bool forceStrict = true; // wait till previous sends are complete
    portMicrophone->write(forceStrict);
    return true;
  }
  
  inline bool writeBattery(const int battery) {
    Bottle &batteryPrepare = portBattery->prepare();
    batteryPrepare.clear();
    batteryPrepare.addInt(battery);
    bool forceStrict = true; // wait till previous sends are complete
    portBattery->write(forceStrict);
    return true;
  }
  
  inline bool writeLight(const int light) {
    Bottle &lightPrepare = portLight->prepare();
    lightPrepare.clear();
    lightPrepare.addInt(light);
    bool forceStrict = true; // wait till previous sends are complete
    portLight->write(forceStrict);
    return true;
  }
  
  inline bool writeInfrared(const int infrared) {
    Bottle &infraredPrepare = portInfrared->prepare();
    infraredPrepare.clear();
    infraredPrepare.addInt(infrared);
    bool forceStrict = true; // wait till previous sends are complete
    portInfrared->write(forceStrict);
    return true;
  }
  
  inline bool writeMotorCurrent1(const int motorcurrent1) {
    Bottle &motorcurrent1Prepare = portMotorCurrent1->prepare();
    motorcurrent1Prepare.clear();
    motorcurrent1Prepare.addInt(motorcurrent1);
    bool forceStrict = true; // wait till previous sends are complete
    portMotorCurrent1->write(forceStrict);
    return true;
  }
  
  inline bool writeMotorCurrent2(const int motorcurrent2) {
    Bottle &motorcurrent2Prepare = portMotorCurrent2->prepare();
    motorcurrent2Prepare.clear();
    motorcurrent2Prepare.addInt(motorcurrent2);
    bool forceStrict = true; // wait till previous sends are complete
    portMotorCurrent2->write(forceStrict);
    return true;
  }
  
  inline bool writeWheel1(const int wheel1) {
    Bottle &wheel1Prepare = portWheel1->prepare();
    wheel1Prepare.clear();
    wheel1Prepare.addInt(wheel1);
    bool forceStrict = true; // wait till previous sends are complete
    portWheel1->write(forceStrict);
    return true;
  }
  
  inline bool writeWheel2(const int wheel2) {
    Bottle &wheel2Prepare = portWheel2->prepare();
    wheel2Prepare.clear();
    wheel2Prepare.addInt(wheel2);
    bool forceStrict = true; // wait till previous sends are complete
    portWheel2->write(forceStrict);
    return true;
  }
  
  inline bool writeLed1(const int led1) {
    Bottle &led1Prepare = portLed1->prepare();
    led1Prepare.clear();
    led1Prepare.addInt(led1);
    bool forceStrict = true; // wait till previous sends are complete
    portLed1->write(forceStrict);
    return true;
  }
  
  inline bool writeLed2(const int led2) {
    Bottle &led2Prepare = portLed2->prepare();
    led2Prepare.clear();
    led2Prepare.addInt(led2);
    bool forceStrict = true; // wait till previous sends are complete
    portLed2->write(forceStrict);
    return true;
  }
  
  inline bool writeLed3(const int led3) {
    Bottle &led3Prepare = portLed3->prepare();
    led3Prepare.clear();
    led3Prepare.addInt(led3);
    bool forceStrict = true; // wait till previous sends are complete
    portLed3->write(forceStrict);
    return true;
  }
  
};
}

