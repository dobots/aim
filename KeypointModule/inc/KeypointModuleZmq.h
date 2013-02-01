#include <KeypointModule.h>

namespace rur {

class KeypointModuleZmq: public KeypointModule {
public:

	KeypointModuleZmq();

	void Prepare();

	void Tick();

	bool Stop();
private:
	void *context;

	void *responder;
};

}
