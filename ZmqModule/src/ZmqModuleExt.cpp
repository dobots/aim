#include <ZmqModuleExt.h>

//#include <stdio.h>
//#include <unistd.h>
#include <string>
#include <iostream>
#include <unistd.h>

using namespace rur;
using namespace std;

static int lifetime = 10;

static const string Value_type_str[] = { "obj_type", "array_type", "str_type", "bool_type", "int_type", "real_type", "null_type" };

/***********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************/

ZmqModuleExt::ZmqModuleExt() {
	socket_state.push_back(true);
	socket_state.push_back(true);
	socket_state.push_back(true);
}

ZmqModuleExt::~ZmqModuleExt() {
	std::cout << "Deallocate object and close socket to name server" << endl;
}

// zmq_ctx_new used to be zmq_init which is by default installed in Ubuntu with apt-get install libzmq-dev
// so you'll need to deinstall that... and install zmq from source
// see: http://zguide.zeromq.org/cpp:hwserver and http://zguide.zeromq.org/cpp:hwclient
// see: http://zguide.zeromq.org/c:hwserver


void ZmqModuleExt::TickClient() {
	sleep(1);
}

void ZmqModuleExt::TickServer() {
	/*
	// Wait for next request from client
	zmq::message_t request;
	std::cout << "Send request" << std::endl;
	socket->recv(&request);
	std::cout << "Received Hello" << std::endl;

	sleep (1);

	// Send reply back to client
	zmq::message_t reply(5);
	memcpy((void*)reply.data(), "World", 5);
	socket->send(reply);
	*/
}

/**
 * This example is from the web, but doesn't seem to work.
 */
bool ZmqModuleExt::Tick() {
//	server ? TickServer() : TickClient();
}

bool ZmqModuleExt::Stop() {
	if (--lifetime) {
		return false;
	}
//	zmq_close (responder);
//	zmq_ctx_destroy (context);
	return true;
}


