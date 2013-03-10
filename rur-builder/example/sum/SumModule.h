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
namespace Rur {

struct Param {
  int sensor_id;
};

typedef std::vector<double> long_seq;

// The generated class. Do not modify or add class members
// Either derive from this class and implement Tick() or
// use a separate helper class to store state information.
// All information for the operation of the module should 
// be obtained over the defined ports
class SumModule {
private:
  Network yarp;
  std::string module_id;
  
  // private storage for portInputValues;
  std::vector<double> *portInputValues;
  // the port portInput itself
  BufferedPort<Bottle> *portInput;
  
  // the port portOutput itself
  BufferedPort<Bottle> *portOutput;
  
  // the port portTest1 itself
  BufferedPort<Bottle> *portTest1;
  
  // private storage for portTest2Value
  int portTest2Value;
  // the port portTest2 itself
  BufferedPort<Bottle> *portTest2;
  
  // User-defined structs (automatically allocated later)
  Param *cliParam;

public:
  // The constructor needs to be called, also when you derive from this class
  SumModule() {
    cliParam = new Param();
    portInputValues = new std::vector<double>();
    portInput = new BufferedPort<Bottle>();
    portOutput = new BufferedPort<Bottle>();
    portTest1 = new BufferedPort<Bottle>();
    portTest2 = new BufferedPort<Bottle>();
  }
  
  ~SumModule() {
    delete portInputValues;
    delete portInput;
    delete portOutput;
    delete portTest1;
    delete portTest2;
    delete cliParam;
  }
  
  // This is the function you will need to implement.
  void Tick(); 
  
  bool Stop(); 
  
  
  // After construction you will need to call this function first
  // it opens the YARP ports
  void Init(std::string module_id) {
    this->module_id = module_id;
    
    {
      std::stringstream portName; portName.str(); portName.clear();
      portName << "/summodule" << module_id << "/input";
      portInput->open(portName.str().c_str());
    }
    {
      std::stringstream portName; portName.str(); portName.clear();
      portName << "/summodule" << module_id << "/output";
      portOutput->open(portName.str().c_str());
    }
    {
      std::stringstream portName; portName.str(); portName.clear();
      portName << "/summodule" << module_id << "/test1";
      portTest1->open(portName.str().c_str());
    }
    {
      std::stringstream portName; portName.str(); portName.clear();
      portName << "/summodule" << module_id << "/test2";
      portTest2->open(portName.str().c_str());
    }
  }
  
  // Before destruction you will need to call this function first
  // it closes the YARP ports
  void Close() {
    portInput->close();
    portOutput->close();
    portTest1->close();
    portTest2->close();
  }
  
  // Function to get Param struct (to subsequently set CLI parameters)
  inline Param *GetParam() { return cliParam; };
  
protected:
  // All subsequent functions should be called from "within" this module
  // From either the Tick() routine itself, or Tick() in a derived class
  
  // Remark: caller is responsible for evoking vector.clear()
  inline std::vector<double> *readInput(bool blocking=true) {
    Bottle *b = portInput->read(blocking);
    if (b != NULL) { 
      for (int i = 0; i < b->size(); ++i) {
        portInputValues->push_back(b->get(i).asDouble());
      }
    }
    return portInputValues;
  }
  
  inline void writeOutput(const int data_out) {
    Bottle &data_outPrepare = portOutput->prepare();
    data_outPrepare.clear();
    data_outPrepare.addInt(data_out);
    portOutput->write(true);
  }
  
  inline void writeTest1(const long_seq &test1) {
    Bottle &test1Prepare = portTest1->prepare();
    test1Prepare.clear();
    for (int i = 0; i < test1.size(); ++i) {
      test1Prepare.addDouble(test1[i]);
    }
    portTest1->write(true);
  }
  
  inline int *readTest2(bool blocking=true) {
    Bottle *b = portTest2->read(blocking);
    if (b != NULL) { 
      portTest2Value = b->get(0).asInt();
      return &portTest2Value;
    }
    return NULL;
  }
  
};
}

