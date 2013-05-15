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

namespace rur {

class DetectLineModuleExt: public DetectLineModule {
public:
	// Initialize
	void Init(std::string & name);

	// The tick function will be called from the DetectLineModuleMain file
	void Tick();

	// As soon as Stop() returns "true", the DetectLineModuleMain will stop the module
	bool Stop();

	//! Load a vector of points from a preprocessed image
	void loadImage(std::string file, std::vector<Point2D> & points);
private:
	dobots::Hough<Point2D> hough;

	//! Flag stops program
	bool stop;

	//! Use simple image structure
	CRawImage *image;

	//! Keep track of number of ticks
	unsigned long tick;
};

}

#endif // DETECTLINEMODULEEXT_H_
