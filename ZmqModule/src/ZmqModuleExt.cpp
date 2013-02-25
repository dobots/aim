#include <ZmqModuleExt.h>

//#include <stdio.h>
//#include <unistd.h>
#include <string>
#include <iostream>
#include <unistd.h>

using namespace rur;
using namespace std;

static int lifetime = 10;

ZmqModuleExt::ZmqModuleExt(): context(1), socket(NULL), server(false) {
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
		std::cout << "Starting name server on port 10101" << std::endl;
		socket = new zmq::socket_t(context, ZMQ_REP);
		socket->bind("tcp://*:10101");
	} else {
		std::cout << "Connecting to name server..." << std::endl;
		socket = new zmq::socket_t(context, ZMQ_REQ);
		socket->connect("tcp://127.0.0.1:10101");

		// Get the reply.
		for (int i = 0; i < channel_count; ++i) {
			std::string name = "/resolve/" + string(channel[i]);
			std::cout << "Acquire TCP/IP port for " << name << std::endl;
			zmq::message_t request (name.size() + 1);
			memcpy ((void *) request.data (), name.c_str(), name.size());
			socket->send(request);

			zmq::message_t reply;
			//reply.rebuild();
			if (!socket->recv (&reply)) continue;
			size_t msg_size = reply.size();
			char* address = new char[msg_size+1];
			memcpy (address, (void *) reply.data(), msg_size);
			address[msg_size] = '\0';
			std::cout << "Received " << string(address) << std::endl;
			delete [] address;

		}
	}
}

void ZmqModuleExt::TickClient() {
	/*
	zmq::message_t request (6);
	memcpy ((void *) request.data (), "Hello", 5);
	std::cout << "Sending Hello " << std::endl;
	socket->send (request);

	sleep (1);

	// Get the reply.
	zmq::message_t reply;
	socket->recv (&reply);
	std::cout << "Received World " << std::endl;
	*/
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


