#include <zmqconnect.h>

#include <zmq.hpp>

namespace rur {

class zmqconnectExt: public zmqconnect {
public:
	zmqconnectExt();

	~zmqconnectExt();

	bool Tick();

	bool Stop();

	inline void SetConnectSource(std::string source) { connect_source = source; }

	inline void SetConnectTarget(std::string target) { connect_target = target; }
private:

	std::string connect_source;

	std::string connect_target;

	zmq::socket_t *cmdc_socket;

	bool success;

	bool ack_received;
};

}
