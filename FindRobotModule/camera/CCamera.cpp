#include "CCamera.h"

#include <cassert>

//-----------------------------------------------------------------------------
// COLOR PART
//-----------------------------------------------------------------------------

#define CLIP(X) ( (X) > 255 ? 255 : (X) < 0 ? 0 : X)

// RGB -> YUV
#define RGB2Y(R, G, B) CLIP(( (  66 * (R) + 129 * (G) +  25 * (B) + 128) >> 8) +  16)
#define RGB2U(R, G, B) CLIP(( ( -38 * (R) -  74 * (G) + 112 * (B) + 128) >> 8) + 128)
#define RGB2V(R, G, B) CLIP(( ( 112 * (R) -  94 * (G) -  18 * (B) + 128) >> 8) + 128)

// YUV -> RGB
#define C(Y) ( (Y) - 16  )
#define D(U) ( (U) - 128 )
#define E(V) ( (V) - 128 )

#define YUV2R(Y, U, V) CLIP(( 298 * C(Y)              + 409 * E(V) + 128) >> 8)
#define YUV2G(Y, U, V) CLIP(( 298 * C(Y) - 100 * D(U) - 208 * E(V) + 128) >> 8)
#define YUV2B(Y, U, V) CLIP(( 298 * C(Y) + 516 * D(U)              + 128) >> 8)

// RGB -> YCbCr
#define CRGB2Y(R, G, B) CLIP((19595 * R + 38470 * G + 7471 * B ) >> 16)
#define CRGB2Cb(R, G, B) CLIP((36962 * (B - CLIP((19595 * R + 38470 * G + 7471 * B ) >> 16) ) >> 16) + 128)
#define CRGB2Cr(R, G, B) CLIP((46727 * (R - CLIP((19595 * R + 38470 * G + 7471 * B ) >> 16) ) >> 16) + 128)

// YCbCr -> RGB
#define CYCbCr2R(Y, Cb, Cr) CLIP( Y + ( 91881 * Cr >> 16 ) - 179 )
#define CYCbCr2G(Y, Cb, Cr) CLIP( Y - (( 22544 * Cb + 46793 * Cr ) >> 16) + 135)
#define CYCbCr2B(Y, Cb, Cr) CLIP( Y + (116129 * Cb >> 16 ) - 226 )

//-----------------------------------------------------------------------------
CCamera::CCamera(sem_t *i)
{
	imSem = i;
	//	initLut();
	return;
}

CCamera::~CCamera()
{
	//	freeLut();
}

int CCamera::init(const char *deviceName,int wi,int he)
{
	printf("Open device %s\n", deviceName);
	height = he;
	width  = wi;
	devfd = open_device();
	init_device();
	start_capturing();
	printf("Open device %s\n", deviceName);
	//OLD:	devfd = opendev(deviceName,width, height, &palette);

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
	printf("Wait for release of image semaphore\n");
	mainloop();
	struct v4lbuffer b = get_result();
	if (b.length != width*height*2) {
		fprintf(stderr, "Length is %i while we would expect (640x480x2=%i)\n", (int)b.length, width*height*2);
	}
	assert (b.length == width*height*2);
	memcpy(buffer,b.start,b.length);
	//OLD:	int ret = grab(devfd, width, height, palette,buffer);
	//	if (ret < 0) {
	//		fprintf(stderr,"Cannot grab a frame from a camera!\n");
	//		return ret;
	//	}
	sem_wait(imSem);
	printf("Within semaphore\n");
	Pyuv422torgb24(buffer,image->data,width,height);
	//	int r = save_jpeg_image(buffer, 480, 640, (char*)"image.jpg");
	sem_post(imSem);
	printf("Image semaphore released\n");
	//	memcpy(image->data,buffer,width*height*2);
	return 0; 
}

/**
 * According to: v4l2-ctl --list-formats
    ioctl: VIDIOC_ENUM_FMT
        Index       : 0
        Type        : Video Capture
        Pixel Format: 'YUYV'
        Name        : YCbCr 4:2:2 Interleaved YUYV
 * This means there should be the pixel format "V4L2_PIX_FMT_YUYV" used in the grab.cpp file. Hence, here we also use
 * it in that order: Y0 U Y1 V.
 *
 * There is no gamma correction in this function
 */

unsigned int CCamera::Pyuv422torgb24(unsigned char * input_ptr, unsigned char * output_ptr,
		unsigned int image_width, unsigned int image_height)
{

	unsigned int i, size;
	unsigned char Y0, Y1, U, V;
	unsigned char *buff = input_ptr;
	unsigned char *output_pt = output_ptr;
	size = image_width * image_height /2;  // make the size 640x480 / 2 and fill output with buffer of size 6x
	for (i = size; i > 0; i--) {
		if (pixel_format == V4L2_PIX_FMT_YUYV) {
			/* bgr instead rgb ?? */
			Y0 = buff[0];
			U = buff[1];
			Y1 = buff[2];
			V = buff[3];
		} else if (pixel_format == V4L2_PIX_FMT_UYVY) {
			U = buff[0];
			Y0 = buff[1];
			V = buff[2];
			Y1 = buff[3];
		} else if (pixel_format == V4L2_PIX_FMT_YYUV) {
			Y0 = buff[0];
			Y1 = buff[1];
			U = buff[2];
			V = buff[3];
		}
		buff += 4;

		*output_pt++ = YUV2R(Y0, U, V); // red is at the right place, does have nothing to do with "U", only with Y0 and V
		*output_pt++ = YUV2G(Y0, U, V); // too green
		*output_pt++ = YUV2B(Y0, U, V); // too yellow

		*output_pt++ = YUV2R(Y1, U, V);
		*output_pt++ = YUV2G(Y1, U, V);
		*output_pt++ = YUV2B(Y1, U, V);
	}

	return 0;
} 


