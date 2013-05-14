/**
 * @brief Accumulator data structure for the Hough transform
 * @file Accumulator.cpp
 *
 * This file is created at Almende B.V. It is open-source software and part of the Common
 * Hybrid Agent Platform (CHAP). A toolbox with a lot of open-source tools, ranging from
 * thread pools and TCP/IP components to control architectures and learning algorithms.
 * This software is published under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless,
 * we personally strongly object to this software being used by the military, in factory
 * farming, for animal experimentation, or anything that violates the Universal
 * Declaration of Human Rights.
 *
 * Copyright © 2013 Anne van Rossum <anne@almende.com>
 *
 * @author  Anne C. van Rossum
 * @date    May 7, 2013
 * @project Replicator FP7
 * @company Almende B.V.
 * @case    Artificial Intelligence Framework
 */
#include <Accumulator.h>

#include <vector>

using namespace dobots;

Accumulator::Accumulator(Size size) {
	this->size.x = size.x;
	this->size.y = size.y;
	std::vector<short> dimensions; dimensions.clear();
	dimensions.push_back(size.x);
	dimensions.push_back(size.y);
	init(dimensions);
}

Accumulator::~Accumulator() {
}

void Accumulator::Increment(Coordinates c) {
}

