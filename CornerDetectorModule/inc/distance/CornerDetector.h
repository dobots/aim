/**
 * @brief 
 * @file CornerDetector.h
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
 * Copyright © 2012 Anne van Rossum <anne@almende.com>
 *
 * @author  Anne C. van Rossum
 * @date    Oct 1, 2012
 * @project Replicator FP7
 * @company Almende B.V.
 * @case    modular robotics / sensor fusion
 */


#ifndef CORNERDETECTOR_H_
#define CORNERDETECTOR_H_

// General files
#include <vector>

//#include <common/CRawImage.h>
#include <CRawImage.h>

struct Corner {
	Corner();
	Corner(int x, int y) { this->x = x; this->y = y; }
	int x;
	int y;
};

struct Patch {
	int x;
	int y;
	int width;
	int height;
};

/* **************************************************************************************
 * Interface of CornerDetector
 * **************************************************************************************/

class CornerDetector {
public:
	//! Constructor CornerDetector
	CornerDetector();

	//! Destructor ~CornerDetector
	virtual ~CornerDetector();

	//! Set image for corner detection
	void SetImage(CRawImage *img);

	//! Get all the corners
	void GetCorners(std::vector<Corner> & corners);

	//! Get only the corners in the corresponding patches (rectangular regions)
	void GetCorners(const std::vector<Patch> & patches, std::vector<Corner> & corners);
protected:

private:
	CRawImage *img;

	CRawImage *dx, *dy, *ddx, *ddy, *dxy, *dH;

	int index;
};


#endif /* CORNERDETECTOR_H_ */
