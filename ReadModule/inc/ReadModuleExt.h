#include <ReadModule.h>

#include <zmq.hpp>

namespace rur {

class ReadModuleExt: public ReadModule {
public:
	ReadModuleExt();

	~ReadModuleExt();

	bool Tick();

	bool Stop();

private:

};

}
