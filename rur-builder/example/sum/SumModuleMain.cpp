/**
 * @file SumModuleMain.cpp
 * @brief Sum last two received values and output result
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
 * @date    Nov 4, 2011
 * @project Replicator FP7
 * @company Almende B.V.
 * @case    Artificial Intelligence Framework
 */
#include <SumModule.h>

#include <stdlib.h>
#include <iostream>

using namespace Rur;
using namespace std;

int main(int argc, char *argv[])  {
	SumModule *m = new SumModule();

	if (argc < 2) {
		std::cout << "Use an identifier as argument for this instance" << endl;
		return EXIT_FAILURE;
	}
	std::string identifier = argv[1];
	m->Init(identifier);

	int t = 0;
	do {
		m->Tick();
	} while (++t < 10);

	delete m;

	return EXIT_SUCCESS;
}
