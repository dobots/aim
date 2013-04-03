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

// The generated class. Do not modify or add class members
// Either derive from this class and implement Tick() or
// use a separate helper class to store state information.
// All information for the operation of the module should 
// be obtained over the defined ports
class ReadModule {
private:
  Network yarp;
  std::string module_id;
  
  // private storage for portInputValue
  int portInputValue;
  // the port portInput itself
  BufferedPort<Bottle> *portInput;

public:
  // The constructor needs to be called, also when you derive from this class
  ReadModule() {
    portInput = new BufferedPort<Bottle>();
  }
  
  ~ReadModule() {
    delete portInput;
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
      portName << "/readmodule" << module_id << "/input";
      portInput->open(portName.str().c_str());
    }
  }
  
  // Before destruction you will need to call this function first
  // it closes the YARP ports
  void Close() {
    portInput->close();
  }
  
protected:
  // All subsequent functions should be called from "within" this module
  // From either the Tick() routine itself, or Tick() in a derived class
  
  inline int *readInput(bool blocking=true) {
    Bottle *b = portInput->read(blocking);
    if (b != NULL) { 
      portInputValue = b->get(0).asInt();
      return &portInputValue;
    }
    return NULL;
  }
  
};
}
