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

	void WriteExample();

	void ReadExample();

	bool Stop();
private:
	zmq::context_t context;

	zmq::socket_t *socket;

	std::vector<zmq::socket_t*> sockets;

	std::vector<bool> socket_state;

	std::string name;

	bool server;

	bool connect;
};

}
