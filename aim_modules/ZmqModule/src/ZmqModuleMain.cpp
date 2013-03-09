/**
 * @file ZmqModuleMain.cpp
 * @brief ...
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
 * Copyright © 2012 Your Name <your@email.address>
 *
 * @author  ...
 * @date    ...
 * @company Almende B.V. (or your university, institute or company)
 * @case    Artificial Intelligence Module
 */
#include <ZmqModuleExt.h>

#include <stdlib.h>
#include <iostream>

using namespace rur;
using namespace std;

/**
 * Every module is a separate binary and hence has its own main method. It is recommended
 * to have a version of your code running without any middleware wrappers, so preferably
 * have this file and the ZmqModule header and code in a separate "aim" directory.
 */
int main(int argc, char *argv[])  {
	ZmqModuleExt *m = new ZmqModuleExt();

	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " identifier | command " << endl;
		return EXIT_FAILURE;
	}
	std::string identifier = argv[1];

	bool connect = (identifier.find("connect") != std::string::npos);
	if (connect & argc < 4) {
		std::cout << "Usage: " << argv[0] << " connect /port0 /port1" << endl;
		return EXIT_FAILURE;
	}

	m->Init(identifier);

	if (connect) {
		m->SetConnectSource(argv[2]);
		m->SetConnectTarget(argv[3]);
	}

	do {
		std::cout << "Tick " << getpid() << endl;
		m->Tick();
	} while (!m->Stop()); 

	delete m;

	std::cout << "End of " << identifier << endl;

	return EXIT_SUCCESS;
}
