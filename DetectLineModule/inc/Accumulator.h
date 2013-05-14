/**
 * @brief Accumulator structure for the Hough transform
 * @file Accumulator.h
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

#ifndef ACCUMULATOR_H_
#define ACCUMULATOR_H_

#include <nd-array.hpp>

struct Coordinates {
	int x;
	int y;
};

struct Size: Coordinates {};

/**
 * A cell in the accumulator. This is different per transform, but we will use the same cell for all of them with a
 * flexible number of fields.
 */
struct Cell: Coordinates {
	//! The number of times a given entity (line) is detected.
	int hits;
};

namespace dobots {

/**
 * The Hough transform uses a specific type of data structure that is called the accumulator in this context. It is
 * basically just a discretization of the Hough space. For lines, which have only two parameters, you will need a 2D
 * Hough space.
 */
class Accumulator : public nd_array<Cell,short> {
public:
	//! Default accumulator constructor
	Accumulator(Size size);

	//! Default accumulator destructor
	~Accumulator();

	/**
	 * Increment cell in the accumulator. This assumes that the calling function knows how to map from the point cloud
	 * to the coordinates of a given cell.
	 */
	void Increment(Coordinates c);

	/**
	 * Reset the accumulator, for example when switching from method, but also in some methods you are required to do
	 * so.
	 */
	void Reset();

	inline Size getSize() { return size; }
private:

	Size size;
};

}

#endif /* ACCUMULATOR_H_ */
