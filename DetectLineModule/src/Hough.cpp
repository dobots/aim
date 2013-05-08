/**
 * @brief The Hough Transform can be used to detect lines or planes from point clouds.
 * @file Hough.cpp
 *
 * This file is created at Almende B.V. It is open-source software and part of the Common Hybrid Agent Platform (CHAP).
 * A toolbox with a lot of open-source tools, ranging from thread pools and TCP/IP components to control architectures
 * and learning algorithms. This software is published under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless, we personally strongly object
 * to this software being used by the military, in factory farming, for animal experimentation, or anything that
 * violates the Universal Declaration of Human Rights.
 *
 * Copyright © 2013 Anne van Rossum <anne@almende.com>
 *
 * @author  Anne C. van Rossum
 * @date    May 7, 2013
 * @project Replicator FP7
 * @company Almende B.V.
 * @case    Artificial Intelligence Framework
 */
#include <Hough.h>
#include <Random.h>

/**
 * By default the randomized Hough transform will be used.
 */
Hough::Hough(): type(RANDOMIZED_HOUGH) {
	points.clear();
	Size size;
	size.x = 100;
	size.y = 100;
	accumulator = new Accumulator(size);
}

Hough::~Hough() {
	points.clear();
}

void Hough::addPoint(Point p) {
	points.push_back(p);
}

void Hough::addPoints(std::vector<Point> & point_cloud) {
	points.insert(points.end(), point_cloud.begin(), point_cloud.end());
}

/**
 *
 */
void Hough::doTransform() {
	std::vector<Point> random_set;
	random_n(points.begin(), points.end(), random_set.begin(), 2);

//	points.
//	random_sh
}

/**
 * We will erase all elements, and because the points are actual values and no pointers this will erase all points.
 */
void Hough::clear() {
	points.clear();
}
