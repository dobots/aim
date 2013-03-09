/**
 * @file DotRobotSensors.h
 * @brief The . robot has a suite of sensors defined here
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
 * @date    Aug 2, 2012
 * @project Replicator FP7
 * @company Almende B.V.
 * @case    Artificial Intelligence Framework
 */

#ifndef DOTROBOTSENSORS_H_
#define DOTROBOTSENSORS_H_

#include <vector>
#include <sstream>
#include <string>
#include <locale>
#include <functional>
#include <iomanip>
#include <fstream>
#include <istream>

/**
 * The sensors and the index in the raw array returned from the dotty robot.
 */
enum DotRobotSensor {
		Counter=-1, Microphone=0, BatteryLevel=1, LightIntensity=2, Infrared=3, MotorCurrent1=4, MotorCurrent2=5,
		Wheel1=6, Wheel2=7, Led1=8, Led2=9, Led3=10 };


/**
 * Local class that knows that colons can be treated as white spaces. It is used by
 * the input stream operator of the RobotSensors class (see below)
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

/**
 * The protocol version that stream in ASCII from the dotty robot does (for now) only
 * stream time, light, and battery values, nothing else.
 */
class RobotSensors {
public:
	//! Timestamp is just a counter, the dotty does not have real time
	double time;

	//! The light sensor
	int light;

	//! The battery level (can be adjusted by potential resistor)
	int battery;

	//! Timestamp in string format
	std::string s_time;

	//! Light sensor value in string format
	std::string s_light;

	//! Battery level in string format
	std::string s_battery;

	// Generate strings from ascii stream
	friend std::istream& operator>>( std::istream& is, RobotSensors& r) {
		is.imbue(std::locale(std::locale(), new tabsep));
		is >> r.s_time >> r.s_light >> r.light >> r.s_battery >> r.battery;
		return is;
	}
};


#endif /* DOTROBOTSENSORS_H_ */
