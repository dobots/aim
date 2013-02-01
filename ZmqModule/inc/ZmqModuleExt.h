#include <ZmqModule.h>

namespace rur {

class ZmqModuleExt: public ZmqModule {
public:

	ZmqModuleExt();

	void Prepare();

	void Tick();

	bool Stop();
private:
	void *context;

	void *responder;
};

}
