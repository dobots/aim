/**
 * @brief 
 * @file CornerDetector.cpp
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

/* **************************************************************************************
 * Configuration options
 * **************************************************************************************/

// Turn on/off saving images to disk
#define STORE_IMAGES		1

/* **************************************************************************************
 * Includes and typedefs
 * **************************************************************************************/

#include <CornerDetector.h>
#include <iostream>
#include <sstream>
#include <convolve.h>
#include <CRawImage.h>
#include <cassert>

#if STORE_IMAGES == 0
#undef STORE_IMAGES
#endif

// pixel values are 8 bit fields (uint8_t would require <inttypes.h>)
#define PIXELTYPE unsigned char

using namespace std;

/* **************************************************************************************
 * Implementation of CornerDetector
 * **************************************************************************************/

CornerDetector::CornerDetector(): img(NULL), dx(NULL), dy(NULL), ddx(NULL), ddy(NULL), dxy(NULL), dH(NULL), index(0) {

}

CornerDetector::~CornerDetector() {

}

/**
 * The image handed over will never be deallocated. However, when you provide a new
 * image with different dimensions, the internal data structures will be deleted and
 * build up again.
 */
void CornerDetector::SetImage(CRawImage *img) {
	assert (img->isMonochrome());
	if (this->img == NULL) {
		this->img = img;
		goto create_temps;
	}

	// we do not need to deallocate if new image is same size as old one
	if (img->size == this->img->size) {
		cout << "Images are same size as old ones: won't reallocate memory" << endl;
		return;
	}

	// delete all temporary images and create anew
	cout << "Delete old temporary images" << endl;
	delete [] dx;
	delete [] ddx;
	delete [] ddy;
	delete [] dxy;
	delete [] dH;
create_temps:
	cout << "Create new temporary images" << endl;
	dx = new CRawImage(img->width, img->height, 1);
	ddx = new CRawImage(img->width, img->height, 1);
	ddy = new CRawImage(img->width, img->height, 1);
	dxy = new CRawImage(img->width, img->height, 1);
	dH = new CRawImage(img->width, img->height, 1);
}

/**
 * www.aishack.in/2010/05/the-shi-tomasi-corner-detector/
 *
 * Get the derivatives
 * http://www.csse.uwa.edu.au/~pk/research/matlabfns/Spatial/derivative5.m
 * Requires a 2D convolution filter.
 *
 * Use a Gaussian filter
 * http://www.csse.uwa.edu.au/~pk/research/matlabfns/Spatial/harris.m
 */
void CornerDetector::GetCorners(std::vector<Corner> & corners) {
	cout << __func__ << ": start" << endl;
	if (!img) {
		cerr << __func__ << "First set image" << endl;
		assert(false);
	}
	assert (img->isMonochrome());

	//5-tap derivative coefficients if only 1 derivative is used
	//	float p[] = {0.037659,  0.249153,  0.426375,  0.249153,  0.037659};
	//	float d1[] = {0.109604,  0.276691,  0.000000, -0.276691, -0.109604};

	//5-tap derivative coefficients for 2 derivatives
	float p[]  = {0.030320,  0.249724,  0.439911,  0.249724,  0.030320};
	float d1[] = {0.104550,  0.292315,  0.000000, -0.292315, -0.104550};
	float d2[] = {0.232905,  0.002668, -0.471147,  0.002668,  0.232905};

	cout << __func__ << ": convolve" << endl;
	bool success;
	success = convolve2DSeparable(img->data, dx->data, img->width, img->height, p, 5, d1, 5);
	assert (success);
	// we don't need dy
	cout << __func__ << ": convolve" << endl;
	convolve2DSeparable(img->data, ddx->data, img->width, img->height, p, 5, d2, 5);
	cout << __func__ << ": convolve" << endl;
	convolve2DSeparable(img->data, ddy->data, img->width, img->height, d2, 5, p, 5);
	cout << __func__ << ": convolve" << endl;
	convolve2DSeparable(dx->data, dxy->data, img->width, img->height, d1, 5, p, 5);

	// We now have the "structure tensor" http://en.wikipedia.org/wiki/Corner_detection
	// or in other wards the "Harris matrix"

	// I don't know why there is first a convolution according to the Shi and Tomasi
	// operator and afterwards a convolution with a Gaussian filter.
	// A 2D Gaussian filter http://www.librow.com/articles/article-9 is also separable
	// and it would be better to incorporate that at once in the Hessian itself
	// for now I just skip it

	float k = 0.005; // 0.04 to 0.15 according to wikipedia

	// now we have to calculate \gamma_1 * \gamma_2 - k ( \gamma_1 + gamma_2 )^2
	// this does not need to calculate the eigenvalue decomposition of A, but only the determinant and the trace
	// it is also possible to avoid to have a parameter k, by using a harmonic mean of the eigenvalues

#ifdef HARRIS_STEPHEN
	cout << __func__ << ": write dH" << endl;
	for (int i = 0; i < img->size; ++i) {
		dH->data[i] = ddx->data[i]*ddy->data[i] - dxy->data[i]*dxy->data[i] \
				- k * (ddx->data[i]+ddy->data[i])*(ddx->data[i]+ddy->data[i]);
	}
#else
	float eps = 0.001;
	for (int i = 0; i < img->size; ++i) {
		dH->data[i] = ddx->data[i]*ddy->data[i] - dxy->data[i]*dxy->data[i] \
				/ (ddx->data[i]+ddy->data[i] + eps);
	}
#endif
	// http://www.csse.uwa.edu.au/~pk/research/matlabfns/Spatial/nonmaxsuppts.m

	// maybe make another copy, now data at (t) can not be overwritten on the spot
	// because we need it for (t-1), hence we do not make use of (t-1) data as in integral images
	// for our neighbourhood calculations
	// however... it seems to suck to do this large loop over all pixels for a given neighbourhood...
	int threshold = 180;
	for (int i = 1; i < img->width-1; ++i) {
		for (int j = 1; j < img->height-1; ++j) {
			int total = dH->data[i+j*img->width] +
//					dH->data[i-1+j*img->width] +
					dH->data[i+1+j*img->width] +
//					dH->data[i+(j-1)*img->width] +
					dH->data[i+(j+1)*img->width] +
//					dH->data[i-1+(j-1)*img->width] +
					dH->data[i+1+(j+1)*img->width]; // +
//					dH->data[i+1+(j-1)*img->width] +
//					dH->data[i-1+(j+1)*img->width];
			if (total > threshold*4) {
//				dH->data[i+j*img->width] = 255;
//				dH->data[i-1+j*img->width] = 255;
//				dH->data[i-1+(j-1)*img->width] = 255;
//				dH->data[i+(j-1)*img->width] = 255;
				dH->data[i+j*img->width] = 100;
				corners.push_back(Corner(i,j));
			}
			else dH->data[i+j*img->width] = 0;
		}
	}


#ifdef STORE_IMAGES
	cout << __func__ << ": save bmp" << endl;
//	dx->saveBmp("dx.bmp");
//	ddx->saveBmp("ddx.bmp");
//	ddy->saveBmp("ddy.bmp");
//	dxy->saveBmp("dxy.bmp");
	stringstream f; f.clear(); f.str("");
	f << "dH_" << ++index << ".bmp";
	dH->saveBmp(f.str().c_str());
#endif

	// Shi and Tomasi computes min(\gamma_1,\gamma_2) (the minimum of the eigenvalues)
	// which seems to be better, but is computationally more expensive than determining the determinant and trace

	cout << __func__ << ": end" << endl;


}


