/*
 * File name: CCamera.h
 * Author:    Tom Krajnik
 */

#ifndef __CCAMERA_H__
#define __CCAMERA_H__

#include "CRawImage.h" 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "grab.h"
#include "color.h"
#include <semaphore.h>


//-----------------------------------------------------------------------------
// Class CCamera
//-----------------------------------------------------------------------------
//! A CCamera class
/*! class to represent robot's camera
 */
class CCamera
{
public:

	CCamera(sem_t *i);
	int init(const char *deviceName,int wi,int he);
	~CCamera();
	int renewImage(CRawImage* image);
	unsigned int Pyuv422torgb24(unsigned char * input_ptr, unsigned char * output_ptr, unsigned int image_width, unsigned int image_height);
private:
	int resolution, format;
	int height, width;
	int frames, devfd, palette;
	unsigned char *buffer;
	sem_t  *imSem;
};
#endif
/* end of CCamera.h */
