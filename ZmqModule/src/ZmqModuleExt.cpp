#include <ZmqModuleExt.h>

//#include <stdio.h>
//#include <unistd.h>
#include <string>
#include <iostream>
#include <unistd.h>

using namespace rur;
using namespace std;

static int lifetime = 30;

/***********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************/

ZmqModuleExt::ZmqModuleExt() {
}

ZmqModuleExt::~ZmqModuleExt() {
	std::cout << "Deallocate object and close socket to name server" << endl;
}

/**
 * This example is from the web, but doesn't seem to work.
 */
bool ZmqModuleExt::Tick() {
	ZmqModule::Tick();
	// run the binary as "write"
	if ((module_id.find("write") != std::string::npos) && (lifetime < 20)) {
//		std::cout << "write module" << std::endl;
		int value = 666;
		writeOutput(value);
//		std::cout << "Send value " << value << std::endl;
	}
	if ((module_id.find("read") != std::string::npos) && (lifetime < 20)) {
//		std::cout << "read module" << std::endl;
		int value = *readInput(true);
		if (value != 0) std::cout << "Got value " << value << std::endl;
	}
	if (module_id.find("connect") != std::string::npos) {
//		std::cout << "connecting binary" << std::endl;
		// tell source to connect to target
		int len = connect_target.size() + 0; // include null-terminator
		char send [len];
		memcpy(send+1, (void*)connect_target.c_str(), connect_target.size());
		send[0] = 0x01; // prepend command
//		send[len-1] = '\0';
		// get first the pid to construct the name of the control port
		pns_record record;
	    record.name = "/resolve" + connect_source;
	    Resolve(record);

	    record.name = "/resolve/" + record.pid + "/control";
	    Resolve(record);

		// send this command to source socket
	    std::stringstream ss; ss.clear(); ss.str("");
	    ss << "tcp://" << record.host << ":" << record.port;
	    std::string sock = ss.str();
	    zmq::socket_t *cmdc_socket = new zmq::socket_t(*context, ZMQ_REQ);
	    std::cout << "Connect to " << sock << std::endl;
	    cmdc_socket->connect(sock.c_str());
	    bool state = true;
	    SendRequest(cmdc_socket, state, true, send);
	    if (state) {
	    	std::cout << "Send successfully" << std::endl;
	    	sleep(1);
	    	delete cmdc_socket;
	    	lifetime = 1;
	    } else {
	    	std::cout << "Message not arrived yet" << std::endl;
	    }
	}
	sleep(1);
	return true;
}

bool ZmqModuleExt::Stop() {
	if (--lifetime) {
		return false;
	}
	return true;
}
