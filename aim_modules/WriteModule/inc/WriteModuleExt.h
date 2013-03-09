#include <WriteModule.h>

#include <zmq.hpp>

namespace rur {

class WriteModuleExt: public WriteModule {
public:
	WriteModuleExt();

	~WriteModuleExt();

	bool Tick();

	bool Stop();

	inline void SetConnectSource(std::string source) { connect_source = source; }

	inline void SetConnectTarget(std::string target) { connect_target = target; }
private:

	std::string connect_source;

	std::string connect_target;
};

}
