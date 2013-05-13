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

#include <sstream>

using namespace rur;

FindRobotModuleExt::FindRobotModuleExt(): cam(NULL), image0(NULL), stop_flag(true), counter(0) {
	from_file = false;
}

FindRobotModuleExt::~FindRobotModuleExt() {
	// already deallocated in Stop()
}

void FindRobotModuleExt::Init(std::string & name) {
	FindRobotModule::Init(name);
	image0 = new CRawImage(640,480,3);

	if (from_file) {
		return;
	}
	sem_init(&imageSem,0,1);

	std::string devStrName = "/dev/video0";
	const char *devName = devStrName.c_str();
	std::cout << "We will use " << std::string(devName) << " as argument" << std::endl;
	std::cout << " make sure you have called modprobe ... etc. before" << std::endl;
	cam = new CCamera(&imageSem);
	cam->init(devName,640,480);

	counter = 0;
}

void FindRobotModuleExt::Patches(std::vector<Patch*> & patches) {
	//image0
	//32/24
	int div = 4;
	for (int i = 0; i < div*div; ++i) {
		Patch *patch = new Patch;
		patch->width = image0->getwidth() / div;
		patch->height = image0->getheight() / div;
		image0->getPatch((i%div)*patch->width,(i/div)*patch->height,*patch);
		patches.push_back(patch);
		CRawImage *img = image0->patch2Img(*patch);
		std::ostringstream ss; ss.clear(); ss.str("");
		ss << "patch" << i << ".bmp";
		img->saveBmp(ss.str().c_str());
		delete img;
	}
}

//! Replace with your own functionality
void FindRobotModuleExt::Tick() {
	FindRobotModule::Tick();
	std::vector<Patch*> patches;
	patches.clear();
	if (from_file) {
		std::ostringstream ss; ss.clear(); ss.str("");
		ss << "/data/blackfin/img/";
		ss << "test0.bmp";
		std::string file = ss.str();
		image0->loadBmp(file.c_str());
		Patches(patches);
	} else {
		std::cout << "Renew image object with data from camera" << std::endl;
		cam->renewImage(image0);
	}
	std::ostringstream ss; ss << "test" << counter << ".bmp";
	std::string bmp_file = ss.str();
	std::cout << "Save image as " << bmp_file << std::endl;
	image0->saveBmp(bmp_file.c_str());
	//image0->makeMonochrome(image0gray);
	usleep(100000); // 0.1 second
	counter++;

	if (counter == 2)
		stop_flag = true;
}

//! Immediately returns true to stop the module
bool FindRobotModuleExt::Stop() {
	if (stop_flag) {
		delete cam;
		delete image0;
	}
	return stop_flag;
}

