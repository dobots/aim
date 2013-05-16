/**
 * @brief Basic image storage and a few functions for manipulation
 * @file CRawImage.h
 *
 * Copyright © 2012 Tom Krajnik
 *
 * @author		Tom Krajnik, Anne C. van Rossum
 * @date		May 14th, 2013
 * @institute	Czech Technical University in Prague
 * @company		Almende B.V.
 * @project		Replicator
 */

#ifndef CIMAGE_H
#define CIMAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char VALUE_TYPE;

/**
 * This is a very basic implementation of an image. It just uses three char's, one for each of the color channels.
 * Nothing fancy, no padding. Moreover, this struct is only used for communication with the user. The internal structure
 * is a (linear) array with RGB values or monochrome values.
 */
struct Pixel {
	VALUE_TYPE r;
	VALUE_TYPE g;
	VALUE_TYPE b;
};

/**
 * The patch is another convenient struct for the user. It allows you to get a patch with data from the image. It is
 * not meant to be super fast, it is just simple. It requires you to set a width and height beforehand, and then it
 * copies the pixels to the data field. This data field does not need to be allocated by you, this will be done in this
 * compilation unit.
 */
struct Patch {
	VALUE_TYPE *data;
	int width;
	int height;
};

/**
 * Really basic image processing data structure. The basic class is created by Tom Krajnik. Extension to switch between
 * color and monochrome, the ability to work with patches, the ability to plot a cross, and the comments, are added by
 * Anne van Rossum.
 */
class CRawImage {
public:

	//! The default constructor requires width, height, and number of bytes per pixel. The latter is only allowed to be
	//! one or three.
	CRawImage(int wi, int he, int bpp);

	//! Default destructor, will free internal "data" if not null.
	~CRawImage();

	//! Copy constructor
	CRawImage(const CRawImage & other);

	//! Gets value at [x,y], only valid if bpp=1
	VALUE_TYPE getValue(int x, int y);

	//! Sets value at [x,y], only valid if bpp=1
	void setValue(int x, int y, VALUE_TYPE value);

	//! Only valid if bpp=3, returns an rgb pixel
	Pixel getPixel(int x, int y);

	//! Returns a pixel in a given patch
	Pixel getPixel(int x, int y, Patch & patch);

	//! Set a pixel in a given patch
	void setPixel(int x, int y, Patch & patch, Pixel pixel);

	//! Set a value in a given patch
	void setValue(int x, int y, Patch & patch, VALUE_TYPE value);

	//! Get the patch itself (requires a malloc op)
	void getPatch(int x, int y, Patch & patch);

	//! Add header information etc. required to make a full-fledged CRawImage from a Patch struct
	CRawImage* patch2Img(Patch &patch);

	//! Reallocate internal data structures if necessary
	void refresh();

	//! Save to file, filename will automatically be appended by index
	void saveBmp(const char* name);

	//! With instances of class CRawImage across multiple binaries, the images will overwrite each other
	void saveNumberedBmp(const char* name, bool increment = true);

	//! Get the last file name
	int getSaveNumber();

	//! Get the last number/index of saved file
	static int numSaved;

	//! Load the image from file again, no index will be appended to the name
	bool loadBmp(const char* name);

	//! Swap the R and B channel if the image contains RGB values
	void swap();

	//! Set the right BMP header, depends on dimensions, bpp, etc.
	void updateHeader();

	//! Plot a vertical and/or horizontal line through x and y (not entirely trivial if bpp > 1)
	void plotLine(int x,int y);
	//! Plot line from point 0 to point 1
	void plotLine(int x0, int y0, int x1, int y1);
	//! Plot blob in center of image
	void plotCenter();
	//! Plot a cross of size "size" at given position
	void plotCross(int i,int j,int size);

	//! Check if image is (already) monochrome
	inline bool isMonochrome() { return (bpp == 1); }

	//! This really throws information away! The grayscale values are according to human sensitivity values (30, 59, 11)
	void makeMonochrome();

	//! Same as makeMonochrome, but keeps image intact, and stores result in parameter "result"
	void makeMonochrome(CRawImage* result);

	//! Get the brightness by iterating over all pixels (expensive)
	double getOverallBrightness(bool upperHalf);

	//! Just show the data to the user, if you screw up, it's your own responsibility
	VALUE_TYPE* data;

	//! Reallocate the internal data structure on changing the number of bytes per pixel
	void setbpp(int bpp) { this->bpp = bpp; refresh(); }

	//! Reallocate the internal data structure on changing the image dimensions
	void setdimensions(int width, int height) { this->width = width; this->height = height; refresh(); }

	//! Get the width in pixels
	const inline int getwidth() { return width; };

	//! Get the height in pixels
	const inline int getheight() { return height; };

	//! Get the size in memory (pixels times the bytes per pixel)
	const inline int getsize() { return size; };
private:
	//! Width of image in pixels
	int width;
	//! Height of image in pixels
	int height;
	//! Bytes per pixel
	int bpp;
	//! Size of data structure (pixels times the bytes per pixel)
	int size;
};

#endif
