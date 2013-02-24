#include <ZmqModuleExt.h>

//#include <stdio.h>
//#include <unistd.h>
#include <string>
#include <iostream>
#include <unistd.h>

using namespace rur;
using namespace std;

static int lifetime = 10;

ZmqModuleExt::ZmqModuleExt(): context(1) {
}

ZmqModuleExt::~ZmqModuleExt() {
	delete socket;
}

// zmq_ctx_new used to be zmq_init which is by default installed in Ubuntu with apt-get install libzmq-dev
// so you'll need to deinstall that... and install zmq from source
// see: http://zguide.zeromq.org/cpp:hwserver and http://zguide.zeromq.org/cpp:hwclient
// see: http://zguide.zeromq.org/c:hwserver
void ZmqModuleExt::Init(std::string & name) {
	this->name = name;
	server = (name.find("server") != std::string::npos);

	if (server) {
		std::cout << "Starting server on port 5555" << std::endl;
		socket = new zmq::socket_t(context, ZMQ_REP);
		socket->bind("tcp://*:5555");
	} else {
		std::cout << "Connecting to server..." << std::endl;
		socket = new zmq::socket_t(context, ZMQ_REQ);
		socket->connect("tcp://127.0.0.1:5555");
	}
}

void ZmqModuleExt::TickClient() {
	zmq::message_t request (6);
	memcpy ((void *) request.data (), "Hello", 5);
	std::cout << "Sending Hello " << std::endl;
	socket->send (request);

	sleep (1);

	// Get the reply.
	zmq::message_t reply;
	socket->recv (&reply);
	std::cout << "Received World " << std::endl;
}

void ZmqModuleExt::TickServer() {
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
}

/**
 * This example is from the web, but doesn't seem to work.
 */
void ZmqModuleExt::Tick() {
	server ? TickServer() : TickClient();
}

bool ZmqModuleExt::Stop() {
	if (--lifetime) {
		return false;
	}
//	zmq_close (responder);
//	zmq_ctx_destroy (context);
	return true;
}


