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
 * Copyright (c) 2012 Anne van Rossum <anne@almende.com>
 *
 * @author  Anne C. van Rossum
 * @company Almende B.V.
 */

#include <string>
#include <vector>

namespace Rur {

struct Param {
  int sensor_id;
};

typedef std::vector<double> long_seq;

class SumModule {
private:
  
  long_seq dummyInput;
  
  
  
  int dummyTest2;
  Param *cliParam;

protected:
  static const int channel_count = 4;
  const char* const channel[4] = {"readInput", "writeOutput", "writeTest1", "readTest2"};

public:
  SumModule() {
    dummyInput = long_seq(0);
    dummyTest2 = int(0);
    cliParam = new Param();
  }
  
  ~SumModule() { }
  
  void Tick();
  
  bool Stop();
  
  void Init(std::string & name) { }
  
  // Function to get Param struct (to subsequently set CLI parameters)
  inline Param *GetParam() { return cliParam; };
  
  inline long_seq *readInput() {
    return &dummyInput;
  }
  
  inline void writeOutput(const int data_out) {
  }
  
  inline void writeTest1(const long_seq test1) {
  }
  
  inline int *readTest2() {
    return &dummyTest2;
  }
  
};
}

