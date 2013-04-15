#ifndef __GRAB_H__
#define __GRAB_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#define V4L_NEW

#ifdef V4L_NEW
#include <libv4l1.h>
#include <libv4l2.h>
#include <linux/videodev2.h>
#else
#include <linux/videodev.h>
#endif

int static pixel_format = V4L2_PIX_FMT_YUYV;
//int static pixel_format = V4L2_PIX_FMT_UYVY;

#define DEBUG

struct v4lbuffer {
	void   *start;
	size_t  length;
};

//#define DEFAULT_FMT VIDEO_PALETTE_UYVY
#define DEFAULT_LEN 16
#define DEFAULT_RATE 60
#define VGA_HEIGHT 480
#define VGA_WIDTH 640
#define QVGA_HEIGHT 240
#define QVGA_WIDTH 320

void mainloop(void);
v4lbuffer get_result(void);
void stop_capturing(void);
void start_capturing(void);
void init_device(void);
int open_device(void);
void close_device(void);

/*
//#ifndef RUNONPC
int try_format(int fd, struct video_picture *pict, int palette, int depth);
//#endif
int try_size(int fd, int width, int height);
//#ifndef RUNONPC
int do_mmap(int fd, struct video_mbuf * pvmbuf, char ** pbuf);
//#endif
int opendev(const char *device, int width, int height, int *palette);
int grab(int devfd, int width, int height, int palette,unsigned char*buffer);
*/

#endif // GRAB__H
