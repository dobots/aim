/**
 * @file CornerDetectorModuleExt.cpp
 * @brief CornerDetectorModule extension
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

#include <iostream>

#include <CornerDetectorModuleExt.h>


using namespace rur;

void CornerDetectorModuleExt::Init(std::string &id) {
	detector = new CornerDetector();
	CRawImage *img = new CRawImage(640,480,3);
	img->loadBmp("../data/fakerobot.bmp");
	img->makeMonochrome();
	detector->SetImage(img);
	detector->GetCorners(corners);
	std::cout << "Discovered " << corners.size() << " corners " << std::endl;
}

//! Replace with your own functionality
void CornerDetectorModuleExt::Tick() {
	std::vector<Corner>::iterator iter;
	std::vector<char> corner_msg; corner_msg.clear();
	for (iter = corners.begin(); iter != corners.end(); ++iter) {
		Corner c = *iter;
		corner_msg.push_back(c.x);
		corner_msg.push_back(c.y);
	}
	std::cout << "Send message of size " << corner_msg.size() << std::endl;
	writePoints(corner_msg);
	stop = true;
}

//! Replace with your own functionality
bool CornerDetectorModuleExt::Stop() {
	if (stop) {
		delete detector;
		return true;
	}
	return false;
}

