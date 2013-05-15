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

using namespace rur;
using namespace dobots;

void DetectLineModuleExt::loadImage(std::string file, std::vector<Point2D> & points) {
	image = new CRawImage(640,480,1);
	if (!image->loadBmp(file.c_str())) {
		std::cerr << "Failed to open file " << file << std::endl;
		return;
	}
	image->saveBmp("test.bmp");
	int margin = 2;
	for (int i = margin; i < (640 - margin); ++i) {
		for (int j = margin; j < (480 - margin); ++j) {
			if (image->getValue(i,j) > 0) {
				points.push_back(Point2D(i,j));
				std::cout << " (" << (int)image->getValue(i,j) << ")[" << i << ',' << j << ']';
			}
		}
	}
	std::cout << std::endl;
	std::cout << "Got " << points.size() << " points out of image " << file << std::endl;
}

/**
 *
 */
void DetectLineModuleExt::Init(std::string & name) {
	std::vector<Point2D> points;
	loadImage("../data/dots.bmp", points);
#ifdef FAKE
	size_t nr_points = 10;
	points.resize(nr_points,Point2D());
	for (int i = 0; i < nr_points; ++i) {
		points[i].x = random_value(0,127);
		points[i].y = random_value(0,127);
	}
#endif
	tick = 0;
	hough.addPoints(points);
	std::cout << "Loaded all points (normally from image processing module)" << std::endl;
}

void DetectLineModuleExt::Tick() {
	hough.doTransform();
	if (++tick == 10) stop = true;
}

bool DetectLineModuleExt::Stop() {
	if (stop) {
		delete image;
		std::cout << "Gracefully stopped" << std::endl;
		return true;
	}
	return false;
}

