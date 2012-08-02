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
#include <algorithm>

#include <BlueSourceModuleClient.h>

#include <DotRobotSensors.h>

using namespace std;

void copy(uint16_t *to, char *from, std::size_t size) {
	int skip = 2;
	for (std::size_t pos = 0; pos != (size<<skip); pos+=skip) {
		to[pos] = (((uint16_t)from[pos]) << 8) + from[pos];
	}
}

//! Helper function that searches for "0xa5 or 165 or 10100101"
int sync(char* buf, int start, int end) {
	for (int i = start; i < end; ++i) {
		if (buf[i] == '\xa5') return i;
	}
	return -1;
}

BlueSourceModuleClient::BlueSourceModuleClient() {

}

BlueSourceModuleClient::~BlueSourceModuleClient() {

}

void BlueSourceModuleClient::Tick() {
	BlueSourceModule::Tick();

	//	ParseSyntaxBart();

	ParseSyntaxLuis();
}

void BlueSourceModuleClient::ParseSyntaxBart() {
	std::string buffer;
	if (!Read(buffer)) return;

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
		writeBattery(s.battery);
	}
}

void BlueSourceModuleClient::ParseSyntaxLuis() {
	int bytes_read = read(s, buf, sizeof(buf));

//	(s);

	//! check for start codon
	int start = sync(buf, 0, bytes_read);
	if (bytes_read <= 0) return;

	if (start <= 0) return;

	if ((bytes_read - start) < 28) {
		cout << "Not enough bytes left" << endl;
	}

	uint16_t buffertemp[28];
	copy(buffertemp, buf+start, bytes_read/2);

//		for (int i = 0; i < bytes_read/2) {
	//
	//	}

	if (bytes_read != 14) {
		cerr << "Wrong number of bytes in buffer " << bytes_read << " instead of 14" << endl;
	}
	RobotSensors s;

	uint16_t buffer[14];
	copy(buffer, buf, 14);

	uint16_t val = buffer[Counter+2];
	cout << "Received message: " << val << endl;

	val = buffer[Microphone+2];
	writeMicrophone(val);

	val = buffer[BatteryLevel+2];
	writeBattery((int)val);

	val = buffer[LightIntensity+2];
	writeLight(val);

	val = buffer[Infrared+2];
	writeInfrared(val);

	val = buffer[MotorCurrent1+2];
	writeMotorCurrent1(val);

	val = buffer[MotorCurrent2+2];
	writeMotorCurrent2(val);

	val = buffer[Wheel1+2];
	writeWheel1(val);

	val = buffer[Wheel2+2];
	writeWheel2(val);

	val = buffer[Led1+2];
	writeLed1(val);

	val = buffer[Led2+2];
	writeLed2(val);

	val = buffer[Led3+2];
	writeLed3(val);
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
		switch(errno) {
		case 111:
			cerr << "Probably a permission issue. Have you ever paired before?" << endl;
			break;
		case 112:
			cerr << "Host is down, in other words: are you sure it's the correct bluetooth address?" << endl;
			break;
		default:
			cerr << "Unknown error" << endl;
			break;
		}
	}

	return (status == 0);
}

/**
 * Just read the bytes from a buffer and drops into a string. If there is nothing to read
 * it returns 0, else way the number of bytes as returned.
 */
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

/**
 * Write in the form of a string to the same buffer as where you can read from, namely the
 * bluetooth connection.
 */
bool BlueSourceModuleClient::Write(const std::string buffer) {
	int status = write(s, buffer.c_str(), buffer.length());
	return (status == 0);
}

/**
 * Close the bluetooth connection.
 */
void BlueSourceModuleClient::Close() {
	close(s);
}
