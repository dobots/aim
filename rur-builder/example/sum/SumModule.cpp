/**
 * @file SumModule.cpp
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
 * @date    Nov 10, 2011
 * @project Replicator FP7
 * @company Almende B.V.
 * @case    Artificial Intelligence Framework
 */

#include <SumModule.h>

using namespace Rur;
using namespace std;

#include <stdlib.h>
#include <iostream>
#include <vector>

/**
 * Calculates the sum over a vector and returns this as a scalar. 
 */
void SumModule::Tick() {
	// does not need to be deallocated, so just dereference and work on it 
	long_seq &input = *readInput();
	int sum = 0;
	for (int i = 0; i < input.size(); ++i) {
		cout << "Read value: " << input[i] << endl;
		sum+=input[i];
	}
	writeOutput(sum);
	cout << "Write value: " << sum << endl;
}
