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
 * @copyright LGPLv3
 */

#include <string>
#include <vector>

namespace rur {

class ReadModule {
private:
  
  int dummyInput;

protected:
  static const int channel_count = 1;
  const char* channel[1];

public:
  ReadModule() {
    const char* const channel[1] = {"readInput"};
    dummyInput = int(0);
  }
  
  ~ReadModule() { }
  
  void Tick() {} 
  
  bool Stop() { return false; }
  
  void Init(std::string & name) { }
  
  // Read from this function and assume it means something
  inline int *readInput(bool blocking_dummy=false) {
    return &dummyInput;
  }
  
};
}

