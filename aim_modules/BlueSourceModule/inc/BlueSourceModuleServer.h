/**
 * @file BlueSourceModuleServer.h
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

#ifndef BLUESOURCEMODULESERVER_H_
#define BLUESOURCEMODULESERVER_H_

#include <string>
#include <BlueSourceModule.h>

/**
 * Class that fulfills the role of a server, connects to a bluetooth device and in the end
 * sends data out over a YARP connection.
 */
class BlueSourceModuleServer: public rur::BlueSourceModule {
public:
	BlueSourceModuleServer();

	~BlueSourceModuleServer();

	void Init();

	//! Returns 0 if there is nothing to read
	int Read(std::string &buffer);

	void Close();
private:
	//! Socket to open (works similar to TCP/IP socket)
	int s;

	//! File descriptor (of client)
	int client;

	//! Read buffer
	char buf[1024];
};


#endif /* BLUESOURCEMODULESERVER_H_ */
