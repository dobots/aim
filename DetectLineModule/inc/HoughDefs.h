/**
 * @brief Common definitions and structures for the Hough transform
 * @file HoughDefs.h
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
 * @date    May 15, 2013
 * @project Replicator FP7
 * @company Almende B.V.
 * @case    Artificial Intelligence Framework
 */


#ifndef HOUGHDEFS_H_
#define HOUGHDEFS_H_

#include <vector>
#include <istream>

typedef int ACCUMULATOR_DATA_TYPE;

const ACCUMULATOR_DATA_TYPE ACCUMULATOR_SIZE_X = 100;
const ACCUMULATOR_DATA_TYPE ACCUMULATOR_SIZE_Y = 100;

enum HoughTransformType { HOUGH, RANDOMIZED_HOUGH, PROB_PROG_HOUGH, SEGMENT_HOUGH, HOUGH_TRANSFORM_TYPE_COUNT };

struct ACoordinates {
	ACCUMULATOR_DATA_TYPE x;
	ACCUMULATOR_DATA_TYPE y;
};

struct ASize: ACoordinates {};

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
 * The point cloud can be in 2D. Stream operators are defined. They are declared as friends in case we want to make the
 * fields x and y private later on.
 */
struct Point2D: Point {
	int x;
	int y;
	Point2D(): x(0), y(0) {}
	Point2D(int x, int y) { this->x = x; this->y = y; }
	void set(int x, int y) { this->x = x; this->y = y; }
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

struct ISize: Point2D { };

struct Segment2D {
	Point2D src;
	Point2D dest;
};

/**
 * A cell in the accumulator. This is different per transform, but we will use the same cell for all of them with a
 * flexible number of fields.
 */
struct Cell {
	//! The number of times a given entity (line) is detected.
	int hits;
	//! Segments that contributed to this cell
	std::vector<Segment2D> segments;
};



#endif /* HOUGHDEFS_H_ */
