#include <WriteModule.h>

namespace rur {

class WriteModuleExt: public WriteModule {
public:
	WriteModuleExt();

	~WriteModuleExt();

	void Tick();

	bool Stop();

private:

};

}
