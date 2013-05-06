#ifndef HOUGH_H_
#define HOUGH_H_

#include <cstring>

#include <nd-array.hpp>

enum HoughTransformType { HOUGH, RANDOMIZED_HOUGH, PROB_PROG_HOUGH, SEGMENT_HOUGH, HOUGH_TRANSFORM_TYPE_COUNT };

struct Cell {
	int x;
	int y;
};

/**
 * The Hough transform uses a specific type of data structure that is called the accumulator in this context. It is
 * basically just a discretization of the Hough space. For lines, which have only two parameters, you will need a 2D
 * Hough space.
 */
class Accumulator : public nd_array<Cell,short> {
public:

};

/**
 * The Hough transform can be used to detect higher-order structures from a bunch of points, commonly called a point
 * cloud. It's first implementation used it to detect lines, we will use it to detect lines first, and planes later.
 */
class Hough {
public:

	Hough();

	virtual ~Hough();

	inline void setType(HoughTransformType type) { this->type = type; }

private:

	HoughTransformType type;
};


#endif // HOUGH_H_
