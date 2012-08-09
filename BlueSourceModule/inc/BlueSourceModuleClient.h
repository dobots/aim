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

#include <BlueSourceModule.h>

/****************************************************************************************************
 * Configuration
 ***************************************************************************************************/

/**
 * There are basically two protocols:
 * - Ascii:   human-friendly formatted string format, basically only sending battery and light values
 * - Binary:  human-unfriendly unformatted binary data in specific format
 */

#define ASCII 			1
#define BINARY 			2

#define PROTOCOL		BINARY

/****************************************************************************************************
 * Use configuration figures
 ***************************************************************************************************/

#if (PROTOCOL == 1)
#include <AsciiStream.h>
#endif
#if (PROTOCOL == 2)
#include <BinaryStream.h>
#endif
/****************************************************************************************************
 * Definition bluetooth client (robot functions as server)
 ***************************************************************************************************/

/**
 * Class that fulfills the role of a bluetooth client, connects to a bluetooth device
 * and in the end sends data out over a YARP connection.
 */
class BlueSourceModuleClient: public rur::BlueSourceModule {
public:
	BlueSourceModuleClient();

	~BlueSourceModuleClient();

	void Tick();

#if (PROTOCOL == 1)
	//! Parse the bluetooth messages (in syntax provided by Bart)
	void ParseSyntaxAscii();
#endif

#if (PROTOCOL == 2)
	//! Parse the bluetooth messages (in syntax provided by Luis)
	void ParseSyntaxBinary();
#endif

	//! Connect to bluetooth device with given address e.g. "01:23:45:67:89:AB"
	bool Init(std::string module_id, std::string device);

	bool Write(const std::string buffer);

	void Close();
private:
	//! Socket to open (works similar to TCP/IP socket)
	int s;

	//! Bluetooth address of device to connect to
	std::string device;

#if (PROTOCOL == 1)
	//! Process ascii stream
	AsciiStream *astream;
#endif
#if (PROTOCOL == 2)
	//! Process binary stream
	BinaryStream *stream;
#endif

};




#endif /* BLUESOURCEMODULECLIENT_H_ */
