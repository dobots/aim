#include "CCamera.h"


//-----------------------------------------------------------------------------
CCamera::CCamera(sem_t *i)
{
	imSem = i;
	initLut();
	return;
}

CCamera::~CCamera()
{
	freeLut();
}

int CCamera::init(const char *deviceName,int wi,int he)
{
	height = he;
	width  = wi;
	printf("Open\n");
	devfd = opendev(deviceName,width, height, &palette);
//	printf("Device opened\n");
//	sleep(1);
	if (!devfd)
	{
		printf("Cannot open video device\n");
		return -1;
	} else {
		printf("Opened device %s\n", deviceName);
	}
//	sleep(1);
	buffer = (unsigned char*)malloc(height*width*2);
	printf("Allocated buffer for camera images\n");
	return -1;
}

int CCamera::renewImage(CRawImage* image)
{
	int ret = grab(devfd, width, height, palette,buffer);
	if (ret < 0) {
		fprintf(stderr,"Cannot grab a frame from a camera!\n"); 
		return ret;
	}
	sem_wait(imSem);
	Pyuv422torgb24(buffer,image->data,width,height);
	sem_post(imSem);
	//	memcpy(image->data,buffer,width*height*2);
	return 0; 
}

unsigned int CCamera::Pyuv422torgb24(unsigned char * input_ptr, unsigned char * output_ptr, unsigned int image_width, unsigned int image_height)
{
	unsigned int i, size;
	unsigned char Y, Y1, U, V;
	unsigned char *buff = input_ptr;
	unsigned char *output_pt = output_ptr;
	size = image_width * image_height /2;
	for (i = size; i > 0; i--) {
		/* bgr instead rgb ?? */
		Y = buff[1] ;
		U = buff[0] ;
		Y1 = buff[3];
		V = buff[2];
		buff += 4;
		*output_pt++ = R_FROMYV(Y,V);
		*output_pt++ = G_FROMYUV(Y,U,V); //b
		*output_pt++ = B_FROMYU(Y,U); //v

		*output_pt++ = R_FROMYV(Y1,V);
		*output_pt++ = G_FROMYUV(Y1,U,V); //b
		*output_pt++ = B_FROMYU(Y1,U); //v
	}

	return 0;
} 


