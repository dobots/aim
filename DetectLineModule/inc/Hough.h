/**
 * @brief Helper functions to pick random elements from std containers
 * @file Random.h
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

#ifndef HOUGH_H_
#define HOUGH_H_

#include <cstring>
#include <vector>

#include <nd-array.hpp>
#include <Accumulator.h>

enum HoughTransformType { HOUGH, RANDOMIZED_HOUGH, PROB_PROG_HOUGH, SEGMENT_HOUGH, HOUGH_TRANSFORM_TYPE_COUNT };

struct Point {};

/**
 * The point clouds can be in 2D.
 */
struct Point2D: Point {
	int x;
	int y;
};

/**
 * The Hough transform can be used to detect higher-order structures from a bunch of points, commonly called a point
 * cloud. It's first implementation used it to detect lines, we will use it to detect lines first, and planes later.
 */
template <typename P>
class Hough {
public:
	//! Default constructor uses a Hough space of size 100x100
	Hough(): type(RANDOMIZED_HOUGH) {
		points.clear();
		Size size;
		size.x = 100;
		size.y = 100;
		accumulator = new Accumulator(size);
	}

	//! Constructor with non-standard size for the Hough space
	Hough(Size size): type(RANDOMIZED_HOUGH) {
		points.clear();
		accumulator = new Accumulator(size);
	}

	//! Default destructor
	virtual ~Hough() {
		clear();
	}

	//! Set the transform, it's your responsible to clear the point cloud and or reset the accumulator
	inline void setType(HoughTransformType type) { this->type = type; }

	//! Add points one by one
	void addPoint(P p) {
		points.push_back(p);
	}

	//! Add points in a bunch, points should just be two or three elements, so by value, not by reference
	void addPoints(std::vector<P> & point_cloud) {
		points.insert(points.end(), point_cloud.begin(), point_cloud.end());
	}

	//! Perform the actual transform on all the points hitherto received
	void doTransform() {
		std::vector<Point> random_set;
		random_set.resize(2, Point());
		random_n(points.begin(), points.end(), random_set.begin(), 2);
	}

	//! Remove all points from the point cloud
	void clear() {
		points.clear();
	}

	//! Get the accumulator, e.g. to reset it
	inline Accumulator* getAccumulator() { return accumulator; }
private:
	//! The type of Hough transform to use
	HoughTransformType type;

	//! Accumulator
	Accumulator *accumulator;

	//! Point cloud, for now store temporary all points, and only perform transform when doTransform is called
	std::vector<P> points;
};


#endif // HOUGH_H_
