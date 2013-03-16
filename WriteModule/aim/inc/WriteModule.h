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
 * Remarks
 **********************************************************************************************************************/

// zmq_ctx_new used to be zmq_init which is by default installed in Ubuntu with apt-get install libzmq-dev
// so you'll need to deinstall that... and install zmq from source
// see: http://zguide.zeromq.org/cpp:hwserver and http://zguide.zeromq.org/cpp:hwclient
// see: http://zguide.zeromq.org/c:hwserver

/***********************************************************************************************************************
 * Interface
 **********************************************************************************************************************/

// general C/C++ headers
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>

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

// Following structure makes it easier to store state information per socket
typedef struct zmq_socket_ext_t {
	zmq::socket_t *sock;
	std::string name;
	bool ready;
} zmq_socket_ext;

/**
 * The generated class. Do not modify or add class members. Either derive from this class and implement Tick() or use a 
 * separate helper class to store state information. All information for the operation of the module should be obtained 
 * over the defined ports.
 */

class WriteModule {
private:
  // the socket over which is communicated with the name server
  zmq::socket_t *ns_socket;
  // standard control socket over which commands arrive to connect to some port for example
  zmq::socket_t *cmd_socket;
  // standard control socket over which commands arrive to connect to some port for example
  std::vector<zmq_socket_ext*> zmq_sockets;
  
  // the port portOutput itself
  zmq_socket_ext portOutput;

public:
  // The constructor needs to be called, also when you derive from this class
  WriteModule(): ns_socket(NULL), cmd_socket(NULL) {
    context = new zmq::context_t(1);
    debug = 0;
    portOutput.sock = new zmq::socket_t(*context, ZMQ_REQ);
    portOutput.ready = true;
    zmq_sockets.push_back(&portOutput);
  }
  
  ~WriteModule() {
    delete portOutput.sock;
  }
  
  // This is the function you will need to implement. Do not forget to call this parent function.
  void Tick() {
    HandleCommand();
  }
  
  bool Stop(); 
  
  // After construction you will need to call this function first. It binds all the sockets.
  void Init(std::string module_id) {
    this->module_id = module_id;
    
    std::cout << "Connecting to name server..." << std::endl;
    ns_socket = new zmq::socket_t(*context, ZMQ_REQ);
    try {
      ns_socket->connect("tcp://127.0.0.1:10101"); // port to connect to, REQ/REP
    } catch (zmq::error_t & e) {
       std::cerr << "Error: Could not connect to the name server: = " << e.what() << std::endl;
    }
    cmd_socket = new zmq::socket_t(*context, ZMQ_REP);
    
    {
      pns_record record;
      std::stringstream ss; ss.clear(); ss.str("");
      ss << getpid();
      record.name = "/resolve/" + ss.str() + "/control";
      record.pid = ss.str();
      Resolve(record);
      ss.clear(); ss.str("");
      ss << "tcp://" << record.host << ":" << record.port; 
      std::string sock = ss.str(); 
      std::cout << "Bind to socket " << sock << std::endl; 
      cmd_socket->bind(sock.c_str());
    }
    
    {
      // outgoing port, function as server
      std::stringstream portName; portName.str(); portName.clear();
      portName << "/writemodule" << module_id << "/output";
      portOutput.name = portName.str();
      std::string resolve = "/resolve" + portName.str();
      pns_record record;
      record.name = resolve;
      std::stringstream ss; ss.clear(); ss.str("");
      ss << getpid();
      record.pid = ss.str();
      Resolve(record);
    }
  }
  
  // Before destruction you will need to call this function first. It closes the zeromq sockets.
  void Close() {
    ns_socket->close();
    cmd_socket->close();
    portOutput.sock->close();
  }
  
protected:
  // the standard zeromq context object
  zmq::context_t *context;
  // the user-defined id of a module
  std::string module_id;
  // some default debug parameter
  char debug;
  // All subsequent functions should be called from "within" this module
  // From either the Tick() routine itself, or Tick() in a derived class
  
  /**
   * The output function sends stuff over a zeromq REQ socket. It works as a server. It cannot be blocking because this
   * would make it impossible to receive message on other ports (under which the /pid/control port). It could have been
   * blocking if it is known if it is connected to a REP port (but the connected() function is apparently not meant for
   * that).
   */
  inline bool writeOutput(const int output) {
    // For now only int return values are supported
    std::stringstream ss; ss.clear(); ss.str("");
    ss << output; // very dirty, no endianness, etc, just use the stream operator itself
    bool state = portOutput.ready;
    SendRequest(portOutput.sock, state, false, ss.str());
    if (state) portOutput.ready = false;
    if (!portOutput.ready) {
      bool ack_state = true;
      ReceiveAck(portOutput.sock, ack_state, true);
      if (ack_state) {
        portOutput.ready = true;
        return true;
      }
    }
    return false;
  }
  
  /**
   * The resolve function can be called by modules to get a new socket (and if you want host name and port). It can also
   * be used by the connector, to bind to these previously set up sockets.
   */
  void Resolve(pns_record & record) {
	std::cout << "Acquire TCP/IP port for " << record.name << std::endl;
	std::string reqname = record.name + ':' + record.pid;
	zmq::message_t request (reqname.size() + 1);
	memcpy ((void *) request.data (), reqname.c_str(), reqname.size());
	ns_socket->send(request);

	zmq::message_t reply;
	if (!ns_socket->recv (&reply)) return;
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
		} else if (key == "pid") {
			record.pid = value.get_str();
		}
	}
  }

  void SendAck(zmq::socket_t *s, bool state) {
	std::cout << "Send ACK" << std::endl;
	SendRequest(s, state, true, "ACK");
  }

  bool ReceiveAck(zmq::socket_t *s, bool & state, bool blocking) {
	int reply_size = 0;
	char *reply = GetReply(s, state, blocking, reply_size);
	if (reply == NULL) return false;
	std::string req = std::string(reply);
	delete [] reply;
	if (req.find("ACK") != std::string::npos) {
		if (debug) std::cout << "Got ACK, thanks!" << std::endl;
		return true;
	}
	std::cerr << "Error: got \"" << req << "\", no ACK, state compromised" << std::endl;
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
	} catch (zmq::error_t &e) {
		std::cout << "Error: received zmq::error_t " << e.what() << std::endl;
	}
	if (state) {
		size_t msg_size = reply.size();
		result = new char[msg_size+1];
		memcpy (result, (void *) reply.data(), msg_size);
		result[msg_size] = '\0';
		reply_size = msg_size;
		//std::cout << "Result: \"" << std::string(result) << "\"" << std::endl;
	}
	return result;
  }

  void SendRequest(zmq::socket_t *s, bool & state, bool blocking, std::string str) {
	if (state) {
		zmq::message_t request(str.size()+1);
		memcpy((void *) request.data(), str.c_str(), str.size());
		if (debug) std::cout << "Send request: " << str << std::endl;
		if (blocking)
			state = s->send(request);
		else
			state = s->send(request, ZMQ_DONTWAIT);
	} else {
		std::cout << "Send nothing (still waiting to receive) " << std::endl;
	}
  }

  void HandleCommand() {
    int reply_size = -1;
    bool state = false;
    char *reply = GetReply(cmd_socket, state, false, reply_size);
    if (reply == NULL) return;
    if (reply_size < 2) std::cerr << "Error: Reply is not large for magic header + command string" << std::endl;
    char magic_value = reply[0];
    reply[reply_size-1] = '\0';
    if (magic_value == 0x01) { // connect to command...
        std::string name = std::string(reply+1);
        int pos = name.find("->");
        if (pos == std::string::npos) {
           std::cerr << "Error: no -> separator in connect command" << std::endl;
        }
        std::string source = name.substr(0, pos);
        std::string target = name.substr(pos+2); // todo: 
        std::cout << "Connect from " << source << " to " << target << std::endl;
    	Connect(source, target);
    } else {
        std::cerr << "Error: Unknown command!" << std::endl;
    }
    SendAck(cmd_socket, true);
    delete [] reply;
  }

  void Connect(std::string source, std::string target) {
    zmq::socket_t *s = GetSocket(source);
    pns_record t_record;
    t_record.name = "/resolve" + target;
    Resolve(t_record);
    std::stringstream ss; ss.clear(); ss.str("");
    ss << "tcp://" << t_record.host << ":" << t_record.port; 
    std::string sock = ss.str(); 
    std::cout << "Connect to socket " << sock << std::endl; 
    try {
        s->connect(sock.c_str());
    } catch (zmq::error_t &e) {
        std::cerr << "Error: Could not connect to " << target << ", because: " << e.what() << std::endl;
    }
  }

  zmq::socket_t* GetSocket(std::string name) {
	for (int i = 0; i < zmq_sockets.size(); ++i) {
		if (zmq_sockets[i]->name.find(name) != std::string::npos) return zmq_sockets[i]->sock;
	}
	std::cerr << "Error: socket name could not be found!" << std::endl;
	return NULL;
	//assert(false); // todo, get the previously registered socket by name
  }

};
}

