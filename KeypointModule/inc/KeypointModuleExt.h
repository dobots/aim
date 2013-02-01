#include <KeypointModule.h>

namespace rur {

class KeypointModuleExt: public KeypointModule {
public:

	KeypointModuleExt();

	void Prepare();

	void Tick();

	bool Stop();
private:

};

}
