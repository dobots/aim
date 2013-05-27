/**
 * @file DetectLineModuleExt.cpp
 * @brief DetectLineModule extension
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
 * Copyright © 2013 Anne van Rossum <anne@almende.com>
 *
 * @author  ...
 * @date    ...
 * @company Almende B.V.
 * @case    Artificial Intelligence Framework
 */
#ifndef DETECTLINEMODULEEXT_H_
#define DETECTLINEMODULEEXT_H_

#include <DetectLineModule.h>
#include <Hough.h>

#include <CRawImage.h>

//! Decorated point
struct DecPoint: Point2D {
	DecPoint();
	DecPoint(int x, int y);
	float skew;
	DecPoint *match;
//	int index;

	friend std::ostream& operator<<(std::ostream& os, const DecPoint& p) {
		//os << p.index << ':' << p.x << ',' << p.y;
		os << p.x << ',' << p.y;
		return os;
	}
	friend std::istream& operator>>( std::istream& is, DecPoint& p) {
		is.imbue(std::locale(std::locale(), new commasep));
		is >> p.x >> p.y;
		return is;
	}
};

struct skew_decreasing {
    inline bool operator() (const DecPoint& self, const DecPoint& other) {
        return (self.skew > other.skew);
    }
};

struct skewref_decreasing {
    inline bool operator() (DecPoint* self, DecPoint* other) {
        return (self->skew > other->skew);
    }
};


namespace rur {

//! The different types of line segmentation variants available
enum Segmentation { ALL_POINTS, LONGEST_LINE, DISTRIBUTION_SENSITIVE, GLUE_POINTS, SEGMENTATION_TYPES };
static const std::string SegmentationDescription[] = { "all points", "longest line", "distribution sensitive", "glue points",
	"number of segmentation types" };

/**
 * DetectLineModuleExt extends the public interface DetectLineModule with additional functions and data structs that
 * store the points and segments.
 */
class DetectLineModuleExt: public DetectLineModule {
public:
	//! Use the nd-array also for the point cloud
	typedef nd_array < std::vector<DecPoint*>,short > pointcloud;

	DetectLineModuleExt();

	~DetectLineModuleExt();

	// Initialize
	void Init(std::string & name);

	// The tick function will be called from the DetectLineModuleMain file
	void Tick();

	// As soon as Stop() returns "true", the DetectLineModuleMain will stop the module
	bool Stop();

	//! Get all segments
	void getSegments();

	//! Calculate skew for all points in a cell
	void calculateSkew(Cell<DecPoint> * c);

	void findMatches(Cell<DecPoint> * self, Cell<DecPoint> * other);

	//! Prepare segments
	void prepareSegments();

	//! Add segments that correspond to a given cell
	void addSegments(Cell<DecPoint> & c, std::vector<Segment2D<DecPoint> > & sgmnts);

	//! Load a vector of points from a preprocessed image
	void loadImage(std::string file, std::vector<Point2D> & points);

	//! Load a vector of points from a preprocessed image
	void loadImage(std::string file, pointcloud & spatial_points);

	//! Plot the accumulator values as an image
	void plotAccumulator();

	//! Plot the segments using values in the accumulator
	void plotSegments();
private:
	dobots::Hough<DecPoint> hough;

	//! Flag stops program
	bool stop;

	//! Use simple image structure
	CRawImage *image;

	//! Keep track of number of ticks
	unsigned long tick;

	//! Type of segmentation
	Segmentation segmentation;

	//! Final set of segments, segments store their points by reference
	std::vector<Segment2D<DecPoint> > segments;

};

}

#endif // DETECTLINEMODULEEXT_H_
