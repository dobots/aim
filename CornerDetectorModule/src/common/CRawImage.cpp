#include "CRawImage.h"
#include <cassert>
#include <iostream>

//static unsigned char header[] =  {66,77,54,16,14,0,0,0,0,0,54,0,0,0,40,0,0,0,128,2,0,0,224,1,0,0,1,0,24,0,0,0,0,0,0,16,14,0,18,11,0,0,18,11,0,0,0,0,0,0,0,0,0,0};

#define RGB_HEADER_SIZE	54
#define PALETTE_SIZE		(256*4)
#define GRAY_HEADER_SIZE    (RGB_HEADER_SIZE+PALETTE_SIZE)
#define BITMAP_SIZE		0

/**
 * Use just a raw header so we don't get packed structs etc. By default we will assume an RGB format.
 */
static unsigned char header[] = {66,77, // magic word for BMP
		RGB_HEADER_SIZE,0,0,0, // size of the BMP file (no data is just the header)
		0,0, // application specific
		0,0, // application specific
		54,0,0,0, // offset to the bitmap
		40,0,0,0, // number of bytes in the DIB header
		0,0,0,0, // width in pixels (18,19,20,21)
		0,0,0,0, // height in pixels
		1,0, // number of color planes, 1 is only legal value
		24,0, // 24-bits (number of bits per pixel), by default RGB with each 8 bits
		0,0,0,0, // BI_RGB, no compression
		0,0,0,0, // size of the raw data in the pixel array including padding, including bitmap
		19,11,0,0, //horizontal resolution
		19,11,0,0, //vertical resolution
		0,0,0,0, //number of colors
		0,0,0,0, // 0 means all colors are important
		0,0,255, //red (start of the bitmap)
		255,255,255, //white pixel
		0,0, // padding for 4-byte alignment
		255,0,0, //blue
		0,255,0, //green
		0,0}; // padding

CRawImage::CRawImage(int wi, int he, int bpp): width(wi), height(he), bpp(bpp)
{
	size = bpp*width*height;
	data = (unsigned char*)calloc(size,sizeof(unsigned char));
	updateHeader();
	numSaved = 0;
}

void CRawImage::updateHeader() {
	assert (width > 0);
	int s;
	if (bpp == 3)
		s = height*width*bpp+RGB_HEADER_SIZE;
	else
		s = height*width*bpp+GRAY_HEADER_SIZE;

	header[2] = (unsigned char)s;
	header[3] = (unsigned char)(s >> 8);
	header[4] = (unsigned char)(s >> 16);

	if (bpp == 3) {
		header[10] = (unsigned char)RGB_HEADER_SIZE;
		header[11] = (unsigned char)(RGB_HEADER_SIZE >> 8);
	} else {
		// we make it plus 4*256 values for color pallete in grayscale case
		header[10] = (unsigned char)GRAY_HEADER_SIZE;
		header[11] = (unsigned char)(GRAY_HEADER_SIZE >> 8);
	}

	header[18] = width%256;
	header[19] = width/256;
//		header[18] = 255;
//		header[19] = 255;
	// image height 22,23, 24, 25
	header[22] = height%256;
	header[23] = height/256;
	// planes, 26,27
	// bits (not bytes) per pixel 28,29
	header[28] = (bpp*8)%256;
	header[29] = (bpp*8)/256;
	// compression 30,31,32,33
	//  size of the raw data, 34,35,36,37
	s = height*width*bpp;
	header[34] = s%256;
	header[35] = s >> 8;
	header[36] = s >> 16;
	// horizontal resolution
	if (bpp == 1) {
//		header[38] = 18;
//		header[42] = 18;
		for (int i = 44; i < RGB_HEADER_SIZE; ++i) {
			header[i] = 0;
		}
		header[47] = 1;
		header[51] = 1;
	}
}

int CRawImage::getSaveNumber()
{
	char name[100];
	FILE* file = NULL;
	do{
		sprintf(name,"%04i.bmp",numSaved++);
		file = fopen(name,"r");
	}
	while (file != NULL);
	numSaved--;
	return numSaved;
}

CRawImage::~CRawImage()
{
	if (data != NULL) {
		free(data);
	}
}

void CRawImage::makeMonochrome() {
	assert (bpp == 3);
	assert (data != NULL);
	assert (width > 0);
	printf("Size is %i\n", size);
	swap();

	unsigned char* newData = (unsigned char*)calloc(width*height,sizeof(unsigned char));
	for (int i = 0; i < width*height; ++i) {
		int temp = data[i*3]*30 + data[i*3+1]*59 + data[i*3+2]*11;
		newData[i] = temp / (100);
	}
	free(data);
	bpp = 1;
	size = bpp*width*height;
	printf("New size becomes %i\n", size);
	data = (unsigned char*)calloc(size,sizeof(unsigned char));
	memcpy(data,newData,size);
	free(newData);
	updateHeader();
}

/**
 * Swap the first and third bytes of an YUV or RGB array
 */
void CRawImage::swap()
{
	if (bpp != 3) {
		return;
	}
	unsigned char* newData = (unsigned char*)calloc(size,sizeof(unsigned char));
	int span = width*bpp;
	for (int j = 0;j<height;j++){
		memcpy(&newData[span*j],&data[span*(height-1-j)],span);
		for (int i = 0;i<width;i++){
			unsigned char a = newData[(width*j+i)*3];
			newData[(width*j+i)*3] = newData[(width*j+i)*3+2];
			newData[(width*j+i)*3+2] = a;
		}
	}
	memcpy(data,newData,size);
	free(newData);
}

void CRawImage::saveBmp(const char* inName)
{
	updateHeader();
	std::cout << __func__ << ": save" << std::endl;
	FILE* file = fopen(inName, "wb");
	std::cout << __func__ << ": save2" << std::endl;
	swap();
	fwrite(header,54,1,file);
	if (bpp == 1) {
		unsigned char *palette = (unsigned char*)calloc(PALETTE_SIZE, sizeof(unsigned char));
		// you'll need a color palette for grayscale images.
		for (int i = 0; i < PALETTE_SIZE / 4; ++i) {
			palette[i*4] = palette[i*4+1] = palette[i*4+2] = i;
			palette[i*4+3] = 0;
		}
		fwrite(palette, PALETTE_SIZE, 1, file);
		free(palette);
	}
	fwrite(data,size,1,file);
	swap();
	fclose(file);
}

void CRawImage::saveBmp()
{
	char name[100];
	sprintf(name,"%04i.bmp",numSaved++);
	saveBmp(name);
}


bool CRawImage::loadBmp(const char* inName)
{
	printf("Open file %s\n", inName);

	// we assume that the image to be loaded has 3 bytes per pixel
	if (bpp == 1) {
		// wrong allocated, reallocate
		free(data);
		bpp = 3;
		size = bpp*width*height;
		data = (unsigned char*)calloc(size,sizeof(unsigned char));
		updateHeader();
	}

	FILE* file = fopen(inName,"rb");
	if (file!=NULL)
	{
		size_t n = fread(data,54,1,file);
		if (n == 0) {
			return false;
		}
		bpp = 3;
		size = height*width*bpp;
		n = fread(data,size,1,file);
		if (n == 0) {
//			assert (false);
			return false;
		}
		fclose(file);
		swap();
		return true;
	}
	return false;
}

void CRawImage::plotCenter()
{
	int centerWidth = 20;
	unsigned char color[] = {255,150,150};
	for (int i = -centerWidth;i<centerWidth;i++){
		for (int j =0;j<3;j++){
			data[(width*(height/2+i)+width/2-centerWidth)*3+j] = color[j];
			data[(width*(height/2+i)+width/2+centerWidth)*3+j] = color[j];
			data[(width*(height/2-centerWidth)+width/2+i)*3+j] = color[j];
			data[(width*(height/2+centerWidth)+width/2+i)*3+j] = color[j];
		}
	}
}

void CRawImage::plotLine(int x,int y) {
	int base;
	if (y < 0 || y > height-1) y = height/2;
	if (x < 0 || x > width-1) x = width/2;
	for(int i=0; i < width;i++) {
		if (i == width/2) i++;
		base = (width*y+i)*3;
		data[base+0] = 255;
		data[base+1] = 0;
		data[base+2] = 255;
	}

	for(int j=0;j<height;j++) {
		const int bidx = ((width*j)+x)*3;
		if (j == height/2) j++;
		data[bidx+0] = 255;
		data[bidx+1] = 255;
		data[bidx+2] = 0;
	}
}


/** pocita jas obrazku:
 *  upperHalf == true, pocita se jen z horni poloviny obrazku
 *  upperHalf == false, pocita jen ze spodni poloviny obrazku
 */
double CRawImage::getOverallBrightness(bool upperHalf) {
	int step = 5;
	int sum,num,satMax,satMin,pos;
	sum=num=satMax=satMin=0;
	int limit = 0;
	if (upperHalf) limit = 0; else limit=height/2;
	for (int i = limit;i<height/2+limit;i+=step){
		for (int j = 0;j<width;j+=step){
			pos = (i*width+j)*bpp;
			if (data[pos] >= 250 && data[pos+1] >=250 && data[pos+2] >= 250) satMax++;  
			if (data[pos] <= 25 && data[pos+1] <=25 && data[pos+2] <= 25) satMin++;
			sum+=data[pos] + data[pos+1] + data[pos+2];
			num++;
		}
	}
	return (sum/num/bpp) + (satMax-satMin)*100.0/num;
}




