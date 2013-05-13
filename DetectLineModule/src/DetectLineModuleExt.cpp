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
 * Copyright © 2013 Your Name <your@email>
 *
 * @author	Your Name
 * @date	Current date
 * @company	Your Company
 * @project	Specific Software Project
 */

#include <DetectLineModuleExt.h>
#include <Random.h>
//#include <CImg.h>

using namespace rur;
using namespace dobots;

/**
 *
 */
void DetectLineModuleExt::Init(std::string & name) {
	std::vector<Point2D> points;
	size_t nr_points = 10;
	points.resize(nr_points,Point2D());
	for (int i = 0; i < nr_points; ++i) {
		points[i].x = random_value(0,127);
		points[i].y = random_value(0,127);
	}
	hough.addPoints(points);
	std::cout << "Loaded all points (normally from image processing module)" << std::endl;
}

//! Replace with your own functionality
void DetectLineModuleExt::Tick() {
	hough.doTransform();
	stop = true;
//	switch (strategy) {
//	case NON_MAXIMUM_SUPPRESSION:
//		NonMaximumSuppression();
//	}
}

/**
 * A sliding-window over the image finds the maximum pixel value in a patch and sets all other pixels to zero.
 */
//void DetectLineModuleExt::NonMaximumSuppression() {
//	int window_size;
//	CImg<> N(5,5);
//	cimg_for2x2(img,x,y,0,0,N,float); // loop seems only to work for defined types
//}

//! Replace with your own functionality
bool DetectLineModuleExt::Stop() {
	if (stop) {
		std::cout << "Gracefully stopped" << std::endl;
		return true;
	}
	return false;
}

