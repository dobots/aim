/**
 * @file BlueSourceModuleMain.cpp
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
#include <BlueSourceModuleClient.h>

#include <stdlib.h>
#include <iostream>
//#include <algorithm>

using namespace rur;
using namespace std;

/**
 * Every module is a separate binary and hence has its own main method. It is recommended
 * to have a version of your code running without any middleware wrappers, so preferably
 * have this file and the BlueSourceModule header and code in a separate "aim" directory.
 */
int main(int argc, char *argv[])  {
	BlueSourceModuleClient *m = new BlueSourceModuleClient();

	if (argc < 3) {
		std::cout << "Usage: BlueSourceModuleClient [module_id] [bluetooth_address]" << endl;
		return EXIT_FAILURE;
	}
	std::string identifier = argv[1];

	std::string bluetooth = argv[2];

//	std::string buffer;
//	buffer = "3031080100016523812901216101340131029080000016523912901215401360137036012000001652401290121400135013403408020001652411290121590";
//	replace( buffer.begin(), buffer.end(), '\xa5', '\n' );
//	cout << buffer << endl;
//	return 1;

	bool success = m->Init(identifier, bluetooth);
	if (success) {
		cout << "Connection with device is successfully established" << endl;
	} else {
		cerr << "Error: Connection with device could not be established!" << endl;
	}

	do {
		m->Tick();
	} while (true); 

	delete m;

	return EXIT_SUCCESS;
}
