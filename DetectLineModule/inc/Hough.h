#ifndef HOUGH_H_
#define HOUGH_H_

#include <cstring>

#include <nd-array.hpp>
#include <Accumulator.h>
#include <vector>

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
class Hough {
public:
	//! Default constructor
	Hough();

	//! Default destructor
	virtual ~Hough();

	//! Set the transform, it's your responsible to clear the point cloud and or reset the accumulator
	inline void setType(HoughTransformType type) { this->type = type; }

	//! Add points one by one
	void addPoint(Point p);

	//! Add points in a bunch, points should just be two or three elements, so by value, not by reference
	void addPoints(std::vector<Point> & point_cloud);

	//! Perform the actual transform on all the points hitherto received
	void doTransform();

	//! Remove all points from the point cloud
	void clear();

	//! Get the accumulator, e.g. to reset it
	inline Accumulator* getAccumulator() { return accumulator; }
private:
	//! The type of Hough transform to use
	HoughTransformType type;

	//! Accumulator
	Accumulator *accumulator;

	//! Point cloud, for now store temporary all points, and only perform transform when doTransform is called
	std::vector<Point> points;
};


#endif // HOUGH_H_
