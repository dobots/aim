/**
 * 456789------------------------------------------------------------------------------------------------------------120
 *
 * @brief Artificial Intelligence Module
 * @file ...
 * 
 * This file is created at Almende B.V. and Distributed Organisms B.V. It is open-source software and belongs to a
 * larger suite of software that is meant for research on self-organization principles and multi-agent systems where
 * learning algorithms are an important aspect.
 *
 * This software is published under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless, we personally strongly object
 * against this software being used for military purposes, factory farming, animal experimentation, and "Universal
 * Declaration of Human Rights" violations.
 *
 * Copyright (c) 2013 Your Name <your@email.address>
 *
 * @author    ...
 * @date      Oct 24, 2012
 * @project   ...
 * @company   ...
 * @company   Distributed Organisms B.V.
 * @case      Artificial Intelligence Module
 */

/***********************************************************************************************************************
 * Interface
 **********************************************************************************************************************/

// general C/C++ headers
#include <vector>
#include <string>
#include <sstream>

// middleware specific headers
#include <zmq.hpp>
#include <json_spirit_reader.h>

// recommended namespace: "rur"
// do not forget to add "using namespace rur;" to your .cpp file
namespace rur {

/**
 * Port name service record. This is like a domain name service record, but instead of containing an IP address and an
 * URI, it comes with a "name" that can be resolved as a "host", "port", and "pid". The name is something like "/write",
 * the host something like "127.0.0.1" or "dev.almende.com" (that is resolvable by dns), "port" is a TCP/UDP port, and
 * "pid" is the process identifier.
 */
typedef struct psn_record_t {
	std::string name;
	std::string host;
	std::string port;
	std::string pid;
} pns_record;

struct Param {
  std::string module_id;
  int parameter;
};

/**
 * The generated class. Do not modify or add class members. Either derive from this class and implement Tick() or use a 
 * separate helper class to store state information. All information for the operation of the module should be obtained 
 * over the defined ports.
 */

class ZmqModule {
private:
  // the standard zeromq context object
  zmq::context_t *context;
  // the socket over which is communicated with the name server
  zmq::socket_t *socket;
  // the user-defined id of a module
  std::string module_id;
  
  // private storage for portValueValue
  int portValueValue;
  // the port portValue itself
  zmq::socket_t *portValue;
  // the REQ/REP sockets combined with non-blocking functionality requires the status of portValue
  bool portValueReady;
  
  // User-defined structs (automatically allocated later)
  Param *cliParam;

public:
  // The constructor needs to be called, also when you derive from this class
  ZmqModule(): socket(NULL) {
    context = new zmq::context_t(1);
    cliParam = new Param();
    portValue = new zmq::socket_t(*context, ZMQ_REP);
  }
  
  ~ZmqModule() {
    delete portValue;
    delete cliParam;
  }
  
  // This is the function you will need to implement.
  void Tick(); 
  
  bool Stop(); 
  
  
  // After construction you will need to call this function first. It binds all the sockets.
  void Init(std::string module_id) {
    this->module_id = module_id;
    
    std::cout << "Connecting to name server..." << std::endl;
    socket = new zmq::socket_t(*context, ZMQ_REQ);
    socket->connect("tcp://127.0.0.1:10101"); // port to connect to, REQ/REP
    
    {
      std::stringstream portName; portName.str(); portName.clear();
      portName << "/zmqmodule" << module_id << "/value";
      std::string resolve = "/resolve/" + portName.str();
      pns_record record;
      record.name = resolve;
      Resolve(record);
      std::stringstream ss;
      ss << "tcp://" << record.host << ":" << record.port; 
      std::string sock = ss.str(); 
      std::cout << "Bind to socket " << sock << std::endl; 
      portValue->bind(sock.c_str());
    }
  }
  
  // Before destruction you will need to call this function first. It closes the zeromq sockets.
  void Close() {
    socket->close();
    portValue->close();
  }
  
  // Function to get Param struct (to subsequently set CLI parameters)
  inline Param *GetParam() { return cliParam; };
  
protected:
  // All subsequent functions should be called from "within" this module
  // From either the Tick() routine itself, or Tick() in a derived class
  
  inline int *readValue(bool blocking=true) {
    // For now only int return values are supported
    SendGeneralRequest(portValue, portValueReady);
    int reply_size = -1;
    char *reply = GetReply(portValue, portValueReady, blocking, reply_size);
    if (reply_size < 3) std::cerr << "Error: Reply is not large enough to store an integer!" << std::endl;
    portValueValue = (reply[0] << 8) + reply[1]; // check endianness
    delete [] reply;
    return &portValueValue;
  }
  
  /**
   * The resolve function can be called by modules to get a new socket (and if you want host name and port). It can also
   * be used by the connector, to bind to these previously set up sockets.
   */
  void Resolve(pns_record & record) {
	std::cout << "Acquire TCP/IP port for " << record.name << std::endl;
	zmq::message_t request (record.name.size() + 1);
	memcpy ((void *) request.data (), record.name.c_str(), record.name.size());
	socket->send(request);

	zmq::message_t reply;
	if (!socket->recv (&reply)) return;
	size_t msg_size = reply.size();
	char* address = new char[msg_size+1];
	memcpy (address, (void *) reply.data(), msg_size);
	address[msg_size] = '\0';
	std::string json = std::string(address);
	std::cout << "Received " << json << std::endl;
	delete [] address;

	// get json object
	bool valid;
	json_spirit::Value value;
	if (!json_spirit::read(json, value)) {
		valid = false;
		std::cerr << "Not a json value" << std::endl;
		return;
	}
	if (value.type() != json_spirit::obj_type) {
		std::cerr << "[1] Unexpected object type "" << Value_type_str[value.type()] << """;
		std::cerr << " instead of "" << Value_type_str[json_spirit::obj_type] << """ << std::endl;
		return;
	}

	// a "json_spirit object" is - by default - a vector of json pairs
	json_spirit::Object obj = value.get_obj();

	for( json_spirit::Object::size_type i = 0; i != obj.size(); ++i ) {
		const json_spirit::Pair& pair = obj[i];
		const std::string& key = pair.name_;
		const json_spirit::Value& value = pair.value_;

		if (key == "identifier") {
			// same thing
		} else if (key == "server") {
			record.host = value.get_str();
		} else if (key == "port") {
			record.port = value.get_str();
		}
	}
  }

  void SendGeneralRequest(zmq::socket_t *s, bool state) {
	if (state) {
		std::string str = "REQ";
		zmq::message_t request(str.size()+1);
		memcpy((void *) request.data(), str.c_str(), str.size());
		std::cout << "Send: " << str << std::endl;
		s->send(request);
	} else {
		std::cout << "Send nothing (still waiting to receive) " << std::endl;
	}
  }

  bool ReceiveGeneralRequest(zmq::socket_t *s, bool & state, bool blocking) {
	int reply_size = 0;
	char *reply = GetReply(s, state, blocking, reply_size);
	if (reply == NULL) return false;
	std::string req = std::string(reply);
	if (req == "REQ") {
		return true;
	}
	delete [] reply;
	std::cerr << "Error: no REQ, state compromised" << std::endl;
	return false;
  }

  char* GetReply(zmq::socket_t *s, bool & state, bool blocking, int & reply_size) {
	zmq::message_t reply;
	char* result = NULL;
	reply_size = 0;
	try {
		if (blocking)
			state = s->recv(&reply);
		else
			state = s->recv(&reply, ZMQ_DONTWAIT);
	} catch (error_t) {
		std::cout << "Received something" << std::endl;
	}
	if (state) {
		size_t msg_size = reply.size();
		result = new char[msg_size+1];
		memcpy (result, (void *) reply.data(), msg_size);
		result[msg_size] = '\0';
		reply_size = msg_size;
		std::cerr << "Result" << std::string(result) << std::endl;
	}
	return result;
  }  

};
}

