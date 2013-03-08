#include <ReadModule.h>

#include <zmq.hpp>

namespace rur {

class ReadModuleExt: public ReadModule {
public:
	ReadModuleExt();

	~ReadModuleExt();

	bool Tick();

	bool Stop();

	inline void SetConnectSource(std::string source) { connect_source = source; }

	inline void SetConnectTarget(std::string target) { connect_target = target; }
private:

	std::string connect_source;

	std::string connect_target;
};

}
