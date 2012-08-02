/**
 * @file BlueSourceModuleClient.h
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

#ifndef BLUESOURCEMODULECLIENT_H_
#define BLUESOURCEMODULECLIENT_H_

#include <sstream>
#include <string>
#include <BlueSourceModule.h>
#include <locale>
#include <functional>
#include <iomanip>
#include <fstream>
#include <istream>

/**
 * Local class that knows that colons can be treated as white spaces. It is used by the input stream.
 */
struct tabsep: std::ctype<char> {
	tabsep(): std::ctype<char>(get_table()) {}
	static std::ctype_base::mask const* get_table() {
		static std::vector<std::ctype_base::mask>
		rc(std::ctype<char>::table_size,std::ctype_base::mask());
		rc['\t'] = std::ctype_base::space;
		rc['{'] = std::ctype_base::space;
		rc[' '] = std::ctype_base::space;
		rc[':'] = std::ctype_base::space;
		rc['\n'] = std::ctype_base::space;
		return &rc[0];
	}
};

class RobotSensors {
public:
	double time;

	int light;

	int battery;

	std::string s_time;
	std::string s_light;
	std::string s_battery;
//private:
	// append
	friend std::istream& operator>>( std::istream& is, RobotSensors& r) {
		is.imbue(std::locale(std::locale(), new tabsep));
		is >> r.s_time >> r.s_light >> r.light >> r.s_battery >> r.battery;
//		is >> r.s_time >> r.s_light >> r.s_battery;
		return is;
	}
};

/**
 * Class that fulfills the role of a bluetooth client, connects to a bluetooth device
 * and in the end sends data out over a YARP connection.
 */
class BlueSourceModuleClient: public rur::BlueSourceModule {
public:
	BlueSourceModuleClient();

	~BlueSourceModuleClient();

	void Tick();

	//! Parse the bluetooth messages (in syntax provided by Bart)
	void ParseSyntaxBart();

	//! Parse the bluetooth messages (in syntax provided by Luis)
	void ParseSyntaxLuis();

	//! Connect to bluetooth device with given address e.g. "01:23:45:67:89:AB"
	bool Init(std::string module_id, std::string device);

	//! Returns 0 if there is nothing to read
	int Read(std::string &buffer);

	bool Write(const std::string buffer);

	void Close();
private:
	//! Socket to open (works similar to TCP/IP socket)
	int s;

	//! Bluetooth address of device to connect to
	std::string device;

	//! Read buffer
	char buf[1024];

	//! Input stream
	std::ifstream is;
};




#endif /* BLUESOURCEMODULECLIENT_H_ */
