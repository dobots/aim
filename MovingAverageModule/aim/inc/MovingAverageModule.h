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
 */

#include <string>
#include <vector>

namespace rur {

struct Param {
  std::string module_id;
  int parameter;
};

typedef std::vector<int> long_seq;

class MovingAverageModule {
private:
  
  int dummyInput;
  
  Param *cliParam;

protected:
  static const int channel_count = 2;
  const char* channel[2];

public:
  MovingAverageModule() {
    const char* const channel[2] = {"readInput", "writeAverage"};
    dummyInput = int(0);
    cliParam = new Param();
  }
  
  ~MovingAverageModule() { }
  
  void Tick() {} 
  
  bool Stop() { return false; }
  
  void Init(std::string & name) { }
  
  // Function to get Param struct (to subsequently set CLI parameters)
  inline Param *GetParam() { return cliParam; };
  
  // Read from this function and assume it means something
  inline int *readInput(bool blocking_dummy=false) {
    return &dummyInput;
  }
  
  // Write to this function and assume it ends up at some receiving module
  inline bool writeAverage(const int output) {
    return true;
  }
};
}

