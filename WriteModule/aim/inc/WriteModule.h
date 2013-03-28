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
 * @author Anne C. van Rossum
 * @copyright Distributed Organisms B.V.
 * @date Mar. 27, 2013
 * @license LGPLv3
 */

#include <string>
#include <vector>

namespace rur {

class WriteModule {
private:
  

protected:
  static const int channel_count = 1;
  const char* channel[1];

public:
  WriteModule() {
    const char* const channel[1] = {"writeOutput"};
  }
  
  ~WriteModule() { }
  
  void Tick() {} 
  
  bool Stop() { return false; }
  
  void Init(std::string & name) { }
  
  // Write to this function and assume it ends up at some receiving module
  inline bool writeOutput(const int output) {
    return true;
  }
};
}

