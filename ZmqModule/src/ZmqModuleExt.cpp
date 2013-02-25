#include <ZmqModuleExt.h>

//#include <stdio.h>
//#include <unistd.h>
#include <string>
#include <iostream>
#include <unistd.h>

#include <json_spirit_reader.h>

using namespace rur;
using namespace std;

static int lifetime = 10;


static const string Value_type_str[] = { "obj_type", "array_type", "str_type", "bool_type", "int_type", "real_type", "null_type" };

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
			if (!socket->recv (&reply)) continue;
			size_t msg_size = reply.size();
			char* address = new char[msg_size+1];
			memcpy (address, (void *) reply.data(), msg_size);
			address[msg_size] = '\0';
			std::string json = string(address);
			std::cout << "Received " << json << std::endl;
			delete [] address;

			zmq::socket_t(context, ZMQ_REP);
			bool valid;
			json_spirit::Value value;
			if (!json_spirit::read(json, value)) {
				valid = false;
				std::cerr << "Not a json value" << std::endl;
				return;
			}
			if (value.type() != json_spirit::obj_type) {
				cerr << "[1] Unexpected object type \"" << Value_type_str[value.type()] << "\"";
				cerr << " instead of \"" << Value_type_str[json_spirit::obj_type] << "\"" << endl;
				return;
			}

			// a "json_spirit object" is - by default - a vector of json pairs
			json_spirit::Object obj = value.get_obj();

			std::string json_server;
			std::string json_port;

			for( json_spirit::Object::size_type i = 0; i != obj.size(); ++i ) {
				const json_spirit::Pair& pair = obj[i];
				const string& key = pair.name_;
				const json_spirit::Value& value = pair.value_;
//				cout << "Retrieve json object with name \"" << key << "\"" << " and type " << Value_type_str[value.type()] << endl;

				if (key == "identifier") {
					// same thing
				} else if (key == "server") {
					json_server = value.get_str();
				} else if (key == "port") {
					json_port = value.get_str();
				}
			}

			std::stringstream ss;
			ss << "tcp://" << json_server << ":" << json_port;
			std::string sock = ss.str();
			std::cout << "Bind to socket " << sock << std::endl;
			socket->bind(sock.c_str());
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


