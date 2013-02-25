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
	socket_state.push_back(true);
	socket_state.push_back(true);
	socket_state.push_back(true);
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
	connect = (name.find("connect") != std::string::npos);

	if (server) {
		std::cout << "Starting name server on port 10101" << std::endl;
		socket = new zmq::socket_t(context, ZMQ_REP);
		try {
			socket->bind("tcp://*:10101");
		} catch (error_t) {
			printf ("Error: bind failed: %s\n", strerror (errno));
			return;
		}
	} else {
		std::cout << "Connecting to name server..." << std::endl;
		socket = new zmq::socket_t(context, ZMQ_REQ);
		socket->connect("tcp://127.0.0.1:10101");

		// Get the reply.
		for (int i = 0; i < channel_count; ++i) {
			std::string resolve = "/resolve/" + string(channel[i]);
			//resolve = resolve + '_' + name;
			std::cout << "Acquire TCP/IP port for " << resolve << std::endl;
			zmq::message_t request (resolve.size() + 1);
			memcpy ((void *) request.data (), resolve.c_str(), resolve.size());
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

			zmq::socket_t *s;
			if (connect) {
				s = new zmq::socket_t(context, ZMQ_REP);
				socket_state[i] = false;
			} else {
				s = new zmq::socket_t(context, ZMQ_REQ);
			}
			sockets.push_back(s);
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
			if (connect) {
				std::cout << "Bind to socket " << sock << std::endl;
				sockets[i]->bind(sock.c_str());
			} else {
				std::cout << "Connect to socket " << sock << std::endl;
				sockets[i]->connect(sock.c_str());
			}
		}
	}
}

void ZmqModuleExt::WriteExample() {
	int socket_i = 0;
	zmq::socket_t *s = sockets[socket_i];

	if (socket_state[socket_i]) {
		std::string str = "sensor data...";
		zmq::message_t request(str.size()+1);
		memcpy ((void *) request.data (), str.c_str(), str.size());
		std::cout << "Send: " << str << std::endl;
		s->send (request);
	}

	// Get the reply.
	zmq::message_t reply;
	try {
//		std::cout << "Poll" << std::endl;
		socket_state[socket_i] = s->recv(&reply, ZMQ_DONTWAIT);
	} catch (error_t) {
		std::cout << "Received something" << std::endl;
	}
	if (socket_state[socket_i]) {
		size_t msg_size = reply.size();
		char* result = new char[msg_size+1];
		memcpy (result, (void *) reply.data(), msg_size);
		result[msg_size] = '\0';
		std::string ack = string(result);
		if (ack != "ACK") {
			std::cerr << "Error: no ACK, state compromised" << std::endl;
		}
	}
}

void ZmqModuleExt::ReadExample() {
	int socket_i = 1;
	zmq::socket_t *s = sockets[socket_i];

	if (socket_state[socket_i]) {
		std::string str = "REQ";
		zmq::message_t request(str.size()+1);
		memcpy((void *) request.data(), str.c_str(), str.size());
		std::cout << "Send: " << str << std::endl;
		s->send(request);
	}

	// Get the reply.
	zmq::message_t reply;
	try {
//		std::cout << "Poll" << std::endl;
		socket_state[socket_i] = s->recv(&reply, ZMQ_DONTWAIT);
	} catch (error_t) {
		std::cout << "Received something" << std::endl;
	}
	if (socket_state[socket_i]) {
		size_t msg_size = reply.size();
		char* result = new char[msg_size+1];
		memcpy (result, (void *) reply.data(), msg_size);
		result[msg_size] = '\0';
		std::cerr << "Result" << string(result) << std::endl;
	}
}

void ZmqModuleExt::TickClient() {
	WriteExample();
	ReadExample();
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


