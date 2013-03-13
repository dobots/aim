#include <WriteModule.h>

#include <zmq.hpp>

namespace rur {

class WriteModuleExt: public WriteModule {
public:
	WriteModuleExt();

	~WriteModuleExt();

	bool Tick();

	bool Stop();

private:

};

}
