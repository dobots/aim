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
#include <iostream>
#include <cmath>
#include <cassert>

#include <nd-array.hpp>
#include <Accumulator.h>
#include <HoughDefs.h>

namespace dobots {

/**
 * The Hough transform can be used to detect higher-order structures from a bunch of points, commonly called a point
 * cloud. It's first implementation used it to detect lines, we will use it to detect lines first, and planes later.
 */
template <typename P>
class Hough {
public:
	//! Default constructor expects image points in size of 640x480 and uses a Hough space of size 100x100
	Hough(): type(RANDOMIZED_HOUGH) {
		points.clear();
		ASize size;
		size.x = 100;
		size.y = 100;
		input_size.x = 640;
		input_size.y = 480;
		max_distance = std::sqrt(input_size.x*input_size.x + input_size.y*input_size.y);
		accumulator = new Accumulator(size);
	}

	//! Constructor with non-standard size for the Hough space
	Hough(ISize input_size, ASize hough_space_size): type(RANDOMIZED_HOUGH) {
		points.clear();
		this->input_size = input_size;
		max_distance = std::sqrt(input_size.x*input_size.x + input_size.y*input_size.y);
		accumulator = new Accumulator(hough_space_size);
	}

	//! Default destructor
	virtual ~Hough() {
		clear();
		delete accumulator;
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
		std::vector<P> random_set;
		random_set.resize(2, P());
		random_n(points.begin(), points.end(), random_set.begin(), 2);
		P pnt0 = random_set[0];
		P pnt1 = random_set[1];
		ACoordinates c = transform(pnt0, pnt1);
		getAccumulator()->Increment(c, pnt0, pnt1);
	}

	template <typename P1, typename C1>
	C1 transform(P1 pnt0, P1 pnt1) {
		assert(false); // no general implementation possible
		return pnt0;
	}

	/**
	 * Transform the line through two points to a (theta,r) coordinate pair. The angle theta ranges from 0 to 2*pi
	 * normally, here it is scaled so it fits a "char". The final accumulator which has to store the values is of
	 * limited size, that's why.
	 */
	ACoordinates transform(Point2D pnt0, Point2D pnt1) {
		float theta;
		float r;
		if (pnt0.y == pnt1.y) { // horizontal lines are special, or else division by zero
			theta = (pnt0.y >= 0) ? 2*atan(1) : 6*atan(1);
			r = pnt0.y;
		} else {
			theta = atan(std::abs(pnt0.x - pnt1.x) / std::abs(pnt0.y - pnt1.y));
			// use first point to calculate distance line to origin, could've second point
			float r1 = std::sqrt(pnt0.x*pnt0.x+pnt0.y*pnt0.y);
			float theta1 = asin(std::abs(pnt0.y)/r1);
			float phi1 = std::abs(theta1 - theta);
			r = r1 * cos(phi1);
		}


		// return in Hough coordinates
		ACoordinates result;
		result.x = (r * accumulator->getSize().x) / max_distance; // r scaled with max_dist/max x
		result.y = (theta * accumulator->getSize().y) / (8*atan(1)); // theta scaled with s
		return result;
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

	//! The input points should fall in this range
	ISize input_size;

	//! Max distance (calculated from input_size)
	int max_distance;
};

}

#endif // HOUGH_H_
