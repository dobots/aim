/**
 * @file MovingAverageModuleExt.cpp
 * @brief MovingAverageModule extension
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
 * Copyright © 2013 Your Name <your@email>
 *
 * @author	Your Name
 * @date	Current date
 * @company	Your Company
 * @project	Specific Software Project
 */

#include <MovingAverageModuleExt.h>

using namespace rur;

static int lifetime = 1000;

void MovingAverageModuleExt::Tick() {
	std::cout << '[' << getpid() << "] Tick " << lifetime << std::endl;
	MovingAverageModule::Tick();
	int *value = readInput();
	if (value) {
		message_counter++;
		std::cout << "Messages received: " << message_counter << std::endl;
		double average = (*value + prev_average*(message_counter-1)) \
			/ message_counter;
		std::cout << "Calculate (" << *value << "+" << prev_average << "*" \
			<< message_counter-1 << ") /" << message_counter << "=" \
			<< average << std::endl;
		writeAverage(average);
		prev_average = average;
	}
	sleep(1);
}

bool MovingAverageModuleExt::Stop() {
	if (--lifetime) return false;
	return true;
}

