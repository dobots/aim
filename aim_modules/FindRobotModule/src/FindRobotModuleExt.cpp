/**
 * @file FindRobotModuleExt.cpp
 * @brief FindRobotModule extension
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

#include <FindRobotModuleExt.h>

#include <string>
#include <sstream>
#include <iostream>

using namespace rur;

//! Replace with your own functionality
void FindRobotModuleExt::Tick() {
	std::cout << "We will use /dev/video0 as argument, make sure you have called modprobe ... etc. before" << std::endl;
	const char *devName = std::string("/dev/video0").c_str();
	image0 = new CRawImage(640,480,3);
	sem_t imageSem;
	cam = new CCamera(&imageSem);
	cam->init(devName,640,480);
	cam->renewImage(image0);
	image0->saveBmp("test.bmp");
	stop_flag = true;
	//image0->makeMonochrome(image0gray);

}

//! Immediately returns true to stop the module
bool FindRobotModuleExt::Stop() {
	if (stop_flag) {
		delete cam;
		delete image0;
	}
	return stop_flag;
}

