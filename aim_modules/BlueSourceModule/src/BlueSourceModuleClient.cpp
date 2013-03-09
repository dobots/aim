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
//#include <fcntl.h>
#include <unistd.h>

#include <BlueSourceModuleClient.h>
#include <DotRobotSensors.h>

using namespace std;

/**
 * Helper function that copies and at the same time copes with endianness.
 */
void copy(uint16_t *to, BUFFER_TYPE *from, std::size_t size) {
	int skip = 2;
	for (std::size_t pos = 0; pos != (size<<skip); pos+=skip) {
		to[pos] = (((uint16_t)from[pos]) << 8) + from[pos];
	}
}

BlueSourceModuleClient::BlueSourceModuleClient() {
#if (PROTOCOL == 1)
	astream = new AsciiStream(1024*8); // make buffer of 8kB
#endif
#if (PROTOCOL == 2)
	stream = new BinaryStream(1024*8); // make buffer of 8kB
	stream->SetStartByte('\xa5');
	stream->ExpectedPacketSize(28);
#endif
}

BlueSourceModuleClient::~BlueSourceModuleClient() {

}

/**
 * The normal function to call, it will read over bluetooth in either ascii or binary
 * format.
 */
void BlueSourceModuleClient::Tick() {
	BlueSourceModule::Tick();

#if (PROTOCOL == 1)
	ParseSyntaxAscii();
#endif
#if (PROTOCOL == 2)
	ParseSyntaxBinary();
#endif
}

#if (PROTOCOL == 1)
/**
 * Read in ASCII format (provided by Bart).
 */
void BlueSourceModuleClient::ParseSyntaxAscii() {
	std::string buffer;
	if (!astream->Read(buffer)) return;

	RobotSensors sensors;
	std::stringstream m; m.clear(); m.str("");
	m << buffer;
	m >> sensors;
#ifdef DEBUG
	cout << "Time = " << s.s_time << endl;
	cout << "Time value = " << s.time << endl;
	cout << "Light = " << s.s_light << endl;
	cout << "Light value = " << s.light << endl;
	cout << "Battery = " << s.s_battery << endl;
	cout << "Battery level = " << s.battery << endl;
#endif
	if (sensors.s_battery == "Battery") {
		cout << "Battery level = " << sensors.battery << endl;
		writeBattery(sensors.battery);
	}
}
#endif

#if (PROTOCOL == 2)
/**
 * Read in binary format (provided by Luis).
 */
void BlueSourceModuleClient::ParseSyntaxBinary() {
	BUFFER_TYPE *raw_buf = stream->Read();

	uint16_t buffer[14];
	copy(buffer, raw_buf, 14);

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


	cout << "Wrote all values over yarp" << endl;
	cerr << "Should deallocate message" << endl;
}
#endif

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

#if (PROTOCOL == 1)
	astream->open(s);
#endif
#if (PROTOCOL == 2)
	stream->open(s);
#endif


	return (status == 0);
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
