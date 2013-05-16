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

#include <HoughDefs.h>

using namespace rur;
using namespace dobots;

const int IMG_WIDTH = 640;
const int IMG_HEIGHT = 480;

//! Adjust these figures, make sure patch width times number of patches unequivocally matches the entire width
const int NR_PATCHES_WIDTH = 8;
const int NR_PATCHES_HEIGHT = 8;

const int PATCH_WIDTH = IMG_WIDTH/NR_PATCHES_WIDTH;
const int PATCH_HEIGHT = IMG_HEIGHT/NR_PATCHES_HEIGHT;

const int HIT_THRESHOLD = 20;
const int TOTAL_TICK_COUNT = 100000;

/**
 * Helper function that considers all points above zero in an image to be points detected previously by a corner or
 * feature detector. If all points are thrown in a vector, it is hard to quickly get out points that are in each
 * other's proximity.
 */
void DetectLineModuleExt::loadImage(std::string file, std::vector<Point2D> & points) {
	image = new CRawImage(IMG_WIDTH,IMG_HEIGHT,1);
	if (!image->loadBmp(file.c_str())) {
		std::cerr << "Failed to open file " << file << std::endl;
		return;
	}
	image->saveBmp("test.bmp");

	// add all points to linear array of points, skipping the margins which are most often just artifacts
	int margin = 2;
	for (int i = margin; i < (IMG_WIDTH - margin); ++i) {
		for (int j = margin; j < (IMG_HEIGHT - margin); ++j) {
			if (image->getValue(i,j) > 0) {
				points.push_back(Point2D(i,j));
			}
		}
	}
	std::cout << "Got " << points.size() << " points out of image " << file << std::endl;
}

/**
 * Assumes that the dimensions of the nd-array are previously set. For now we only expect it to be 2D similar to the
 * dimension of the physical pixel array. It becomes "circular" to define proximity in a way that already "knows" their
 * physical 3D location.
 */
void DetectLineModuleExt::loadImage(std::string file, pointcloud & spatial_points) {
	image = new CRawImage(IMG_WIDTH,IMG_HEIGHT,1);
	if (!image->loadBmp(file.c_str())) {
		std::cerr << "Failed to open file " << file << std::endl;
		return;
	}
	image->saveBmp("test.bmp");

	assert (spatial_points.get_dimensions() == 2);

	Patch patch;
	patch.width = IMG_WIDTH / spatial_points.get_dimension(0);
	patch.height = IMG_HEIGHT / spatial_points.get_dimension(1);
	std::cout << "Patch size is " << patch.width << "x" << patch.height << std::endl;

	assert(patch.width == PATCH_WIDTH);
	assert(patch.height == PATCH_HEIGHT);

	// loop over all patches
	for (int x = 0; x != NR_PATCHES_WIDTH; ++x) {
		for (int y = 0; y != NR_PATCHES_HEIGHT; ++y) {
//			std::cout << "Get patch at " << x << "x" << y << std::endl;
			std::vector<Point2D> & v = spatial_points.get(x,y);
			v.clear();
			for (int i = 0; i < PATCH_WIDTH; ++i) {
				int xi = x * PATCH_WIDTH + i;
				for (int j = 0; j < PATCH_HEIGHT; ++j) {
					int yj = y * PATCH_HEIGHT + j;
//					std::cout << "Get value at " << xi << "x" << yj << std::endl;
					if (image->getValue(xi,yj) > 0) {
						v.push_back(Point2D(xi,yj));
					}
				}
			}
		}
	}

	// for debugging purposes
	long int pnt_count = 0;
	for (int x = 0; x != NR_PATCHES_WIDTH; ++x) {
		for (int y = 0; y != NR_PATCHES_HEIGHT; ++y) {
			std::vector<Point2D> & v = spatial_points.get(x,y);
			pnt_count += v.size();
		}
	}
	std::cout << "Got " << pnt_count << " points out of image " << file << std::endl;
}

/**
 *
 */
void DetectLineModuleExt::Init(std::string & name) {
#ifdef LINEAR
	std::vector<Point2D> points;
	loadImage("../data/dots.bmp", points);
	hough.addPoints(points);
#endif
	std::vector<short> dimensions;
	dimensions.push_back(NR_PATCHES_WIDTH);
	dimensions.push_back(NR_PATCHES_HEIGHT);
	pointcloud cloud(dimensions);
	loadImage("../data/square.bmp", cloud);
	hough.addPoints(cloud);

	tick = 0;
	std::cout << "Loaded all points (normally from image processing module)" << std::endl;
}

void DetectLineModuleExt::Tick() {
	hough.doTransform();
	if (++tick == TOTAL_TICK_COUNT) {
		Accumulator & acc = *hough.getAccumulator();
		ASize asize = acc.getSize();
		std::cout << "Create image for accumulator of size " << asize.x << 'x' << asize.y << std::endl;
		CRawImage *a_img = new CRawImage(asize.x, asize.y, 1);
		for (int i = 0; i < asize.x; ++i) {
			for (int j = 0; j < asize.y; ++j) {
				Cell & c = acc.get(i,j);
//				if (c.hits > 0) std::cout << "Hit at " << i << ',' << j << std::endl;
				a_img->setValue(i,j,c.hits > HIT_THRESHOLD ? 100 : 0);
			}
		}
		a_img->saveBmp("acc_img.bmp");
		delete a_img;

		CRawImage *l_img = new CRawImage(IMG_WIDTH, IMG_HEIGHT, 1);
		for (int i = 0; i < asize.x; ++i) {
			for (int j = 0; j < asize.y; ++j) {
				Cell & c = acc.get(i,j);
				if (c.hits > HIT_THRESHOLD) {
//					int last = c.points.size()-1;
//					l_img->plotLine(c.points[0].x, c.points[0].y, c.points[last].x, c.points[last].y);
					for (int l = 0; l < c.points.size()-1; l+=2) {
						l_img->plotLine(c.points[l].x, c.points[l].y, c.points[l+1].x, c.points[l+1].y);
					}
				}
			}
		}
		l_img->saveBmp("line_img.bmp");
		delete l_img;

		stop = true;
	}
}

bool DetectLineModuleExt::Stop() {
	if (stop) {
		delete image;
		std::cout << "Gracefully stopped" << std::endl;
		return true;
	}
	return false;
}

