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

#ifndef CORNERDETECTORMODULE_H_
#define CORNERDETECTORMODULE_H_

#include <string>
#include <vector>

namespace rur {

struct Param {
  std::string module_id;
  char parameter;
};

typedef std::vector<char> char_seq;

class CornerDetectorModule {
private:
  
  char_seq dummyCamera;
  
  Param *cliParam;

protected:
  static const int channel_count = 2;
  const char* channel[2];

public:
  CornerDetectorModule() {
    const char* const channel[2] = {"readCamera", "writePoints"};
    dummyCamera = char_seq(0);
    cliParam = new Param();
  }
  
  ~CornerDetectorModule() { }
  
  void Tick() {} 
  
  bool Stop() { return false; }
  
  void Init(std::string & name) { }
  
  // Function to get Param struct (to subsequently set CLI parameters)
  inline Param *GetParam() { return cliParam; };
  
  // Read from this function and assume it means something
  // Read from this function and assume it means something
  inline char_seq *readCamera(bool blocking_dummy=false) {
    return &dummyCamera;
  }
  
  // Write to this function and assume it ends up at some receiving module
  // Write to this function and assume it ends up at some receiving module
  inline bool writePoints(const char_seq output) {
    return true;
  }
};
}

#endif // CORNERDETECTORMODULE_H_
