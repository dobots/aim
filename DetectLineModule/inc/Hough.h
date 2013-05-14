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

#include <nd-array.hpp>
#include <Accumulator.h>

enum HoughTransformType { HOUGH, RANDOMIZED_HOUGH, PROB_PROG_HOUGH, SEGMENT_HOUGH, HOUGH_TRANSFORM_TYPE_COUNT };

struct Point {};

//! Local class that knows that commas can be treated as white spaces. It is by the input stream operator.
struct commasep: std::ctype<char> {
	commasep(): std::ctype<char>(get_table()) {}
	static std::ctype_base::mask const* get_table() {
		static std::vector<std::ctype_base::mask>
		rc(std::ctype<char>::table_size,std::ctype_base::mask());
		rc[','] = std::ctype_base::space;
		return &rc[0];
	}
};

/**
 * The point cloud can be in 2D.
 */
struct Point2D: Point {
	int x;
	int y;
	Point2D(): x(0), y(0) {}
	Point2D(int x, int y) { this->x = x; this->y = y; }
private:
	friend std::ostream& operator<<(std::ostream& os, const Point2D & p) {
		os << p.x << ',' << p.y;
		return os;
	}
	friend std::istream& operator>>( std::istream& is, Point2D& p) {
		is.imbue(std::locale(std::locale(), new commasep));
		is >> p.x >> p.y;
		return is;
	}
};

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
		Size size;
		size.x = 100;
		size.y = 100;
		input_size.x = 640;
		input_size.y = 480;
		max_distance = std::sqrt(input_size.x*input_size.x + input_size.y*input_size.y);
		accumulator = new Accumulator(size);
	}

	//! Constructor with non-standard size for the Hough space
	Hough(Size input_size, Size hough_space_size): type(RANDOMIZED_HOUGH) {
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
		typename std::vector<P>::iterator iter;
		for (iter = random_set.begin(); iter != random_set.end(); ++iter) {
			std::cout << *iter << std::endl;
		}
		P pnt0 = random_set[0];
		P pnt1 = random_set[1];
		P p = transform(pnt0, pnt1);
		std::cout << "Result: " << p << std::endl;
	}

	template <typename P1>
	P1 transform(P1 pnt0, P1 pnt1) {
		assert(false); // no general implementation possible
		return pnt0;
	}

	/**
	 * Transform the line through two points to a (theta,r) coordinate pair. The angle theta ranges from 0 to 2*pi
	 * normally, here it is scaled so it fits a "char". The final accumulator which has to store the values is of
	 * limited size, that's why.
	 */
	Point2D transform(Point2D pnt0, Point2D pnt1) {
		float theta = atan(std::abs(pnt0.x - pnt1.x) / std::abs(pnt0.y - pnt1.y));

		// use first point
		float r1 = std::sqrt(pnt0.x*pnt0.x+pnt0.y*pnt0.y);
		float theta1 = asin(std::abs(pnt0.y)/r1);
		float phi1 = std::abs(theta1 - theta);
		Point2D result;
		result.x = ((r1 * cos(phi1)) * accumulator->getSize().x) / max_distance; // r scaled with max_dist/max x
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
	Size input_size;

	//! Max distance (calculated from input_size)
	int max_distance;
};

}

#endif // HOUGH_H_
