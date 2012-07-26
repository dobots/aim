/**
 * @file BlueSourceModuleClient.cpp
 * @brief 
 *
 * This file is created at Almende B.V. It is open-source software and part of the Common 
 * Hybrid Agent Platform (CHAP). A toolbox with a lot of open-source tools, ranging from 
 * thread pools and TCP/IP components to control architectures and learning algorithms. 
 * This software is published under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless,
 * we personally strongly object against this software being used by the military, in the
 * bio-industry, for animal experimentation, or anything that violates the Universal
 * Declaration of Human Rights.
 *
 * Copyright © 2010 Anne van Rossum <anne@almende.com>
 *
 * @author  Anne C. van Rossum
 * @date    Jul 26, 2012
 * @project Replicator FP7
 * @company Almende B.V.
 * @case    Artificial Intelligence Framework
 */

#include <errno.h>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <BlueSourceModuleClient.h>

using namespace std;

BlueSourceModuleClient::BlueSourceModuleClient() {

}

BlueSourceModuleClient::~BlueSourceModuleClient() {

}

void BlueSourceModuleClient::Tick() {
	BlueSourceModule::Tick();
	std::string buffer;
	if (Read(buffer) ) {
		// parse this string and write result as integer or something else
		RobotSensors s;
		std::stringstream m; m.clear(); m.str("");
		m << buffer;
		m >> s;
#ifdef DEBUG
		cout << "Time = " << s.s_time << endl;
		cout << "Time value = " << s.time << endl;
		cout << "Light = " << s.s_light << endl;
		cout << "Light value = " << s.light << endl;
		cout << "Battery = " << s.s_battery << endl;
		cout << "Battery level = " << s.battery << endl;
#endif
		if (s.s_battery == "Battery") {
			cout << "Battery level = " << s.battery << endl;
			writeResult(s.battery);
		}
	}
}

/**
 * Try to connect to device and return status. The string "device" should be of
 * the form "01:23:45:67:89:AB"
 */
bool BlueSourceModuleClient::Init(std::string module_id, std::string device) {
	BlueSourceModule::Init(module_id);
	struct sockaddr_rc addr = { 0 };
	int status;
	this->device = device;

	// allocate a socket
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	// set the connection parameters (who to connect to)
	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = (uint8_t) 1;
	str2ba( device.c_str(), &addr.rc_bdaddr );

	// connect to server
	cout << "Connect to server \"" << device << "\"" << endl;
	status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

	// 111: permission issue (which I have with my Android phone e.g.)
	// 112: "host is down" (wrong bluetooth address)
	if (status != 0) {
		cerr << "Error status: " << errno << endl;
	}

	return (status == 0);
}

int BlueSourceModuleClient::Read(std::string &buffer) {
	int bytes_read = read(s, buf, sizeof(buf));
	if( bytes_read > 0 ) {
#ifdef DEBUG
		printf("received [%s]\n", buf);
#endif
		buffer = std::string(buf);
	} else {
		buffer = "";
	}
	return bytes_read;
}

bool BlueSourceModuleClient::Write(const std::string buffer) {
	int status = write(s, buffer.c_str(), buffer.length());
	return (status == 0);
}


void BlueSourceModuleClient::Close() {
	close(s);
}
