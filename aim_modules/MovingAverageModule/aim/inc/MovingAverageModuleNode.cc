#define BUILDING_NODE_EXTENSION
#include <node.h>
//#include <MovingAverageModule.h>
#include <MovingAverageModuleExt.h>

using namespace v8;
using namespace rur;

void RegisterModule(Handle<Object> exports) {
  //MovingAverageModule::NodeRegister(exports);
  MovingAverageModuleExt::NodeRegister(exports);
}

NODE_MODULE(MovingAverageModule, RegisterModule)
