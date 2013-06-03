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

/**
 * The definition of a segment is a bit filthy. It does only except templates which are subsequently used in the form
 * of references. This to make sure there are no copy assignments done by accident.
 */
template <typename P>
struct Segment2D {
	Segment2D(): src(NULL), dest(NULL) {}
	P* src;
	P* dest;
};

/**
 * A cell in the accumulator. This is different per transform, but we will use the same cell for all of them with a
 * flexible number of fields.
 */
template <typename P>
struct Cell {
	//! The number of times a given entity (line) is detected.
	int hits;
	//! Segments that contributed to this cell
	std::vector<Segment2D<P> > segments;
	//! Points themselves, not organized according segments
	std::vector<P*> points;
};

/**
 * Use as sort(points.begin(), points.end(), x_increasing()); This returns an order on the x-coordinate, so if this is a
 * vertical line with one exception in the middle of a point that is slightly off to the right. This point will be
 * singled out. The alternative would be to use some sorting function that minimizes the inter-point distances. This
 * would correspond exactly to an Euclidean traveling salesman problem, a bit of an overkill for most applications.
 */
struct x_increasing {
    inline bool operator() (const Point2D& self, const Point2D& other) {
    	if (self.x == other.x) {
    		return (self.y < other.y); // for points that are exactly vertical, consider the y-coordinate
    	}
        return (self.x < other.x);
    }
};

struct xref_increasing {
    inline bool operator() (Point2D* self, Point2D* other) {
    	if (self->x == other->x) {
    		return (self->y < other->y); // for points that are exactly vertical, consider the y-coordinate
    	}
        return (self->x < other->x);
    }
};

/**
 * How I would be solving the TSP is by using the following method.
 *
 * a.) defining a set of K radial lines
 * b.) representing the points in polar coordinates
 * c.) projecting the points on each of the lines, in other words, rotating the points and ordering them in the line
 *    reference frame
 * d.) for each two subsequent points on each line, cast a vote
 * e.) pick triplets with maximum number of votes out of the array, remove segments that contain the middle point
 * f.) iterate this until all points are gone
 *
 * Probably e and f can again be formulated as a TSP, now with the number of cities equal to K.
 */


#endif /* HOUGHDEFS_H_ */
