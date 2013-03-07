#include <ZmqModule.h>

#include <zmq.hpp>

namespace rur {

class ZmqModuleExt: public ZmqModule {
public:
	ZmqModuleExt();

	~ZmqModuleExt();

//	void Init(std::string & name);

//	void Open(const pns_record & record, bool first);

	bool Tick();

	void TickServer();

	void TickClient();

//	void WriteExample();

//	void ReadExample();

	bool Stop();

	inline void SetConnectSource(std::string source) { connect_source = source; }

	inline void SetConnectTarget(std::string target) { connect_target = target; }
private:
//	std::vector<zmq::socket_t*> sockets;

	std::vector<bool> socket_state;

	std::string name;

//	bool server;

//	bool connect;

	std::string connect_source;

	std::string connect_target;
};

}
