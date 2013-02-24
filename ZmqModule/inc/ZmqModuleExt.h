#include <ZmqModule.h>

#include <zmq.hpp>

namespace rur {

class ZmqModuleExt: public ZmqModule {
public:
	ZmqModuleExt();

	~ZmqModuleExt();

	void Init(std::string & name);

	void Tick();

	void TickServer();

	void TickClient();

	bool Stop();
private:
	zmq::context_t context;

	zmq::socket_t *socket;

	std::string name;

	bool server;
};

}
