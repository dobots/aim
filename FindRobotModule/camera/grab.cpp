/* Capture from a video4linux camera driver
 *
 * <hackfin@section5.ch> explicitely adapted to the OV9655 camera chip
 * in UYVY mode
 *
 * */
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

#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include "grab.h"

int frame_rate = DEFAULT_RATE;

long get_cur_ms()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

//#ifndef RUNONPC
int try_format(int fd, struct video_picture *pict, int palette, int depth)
{
	int err;
	pict->palette=palette;
	pict->depth=depth;
	err=ioctl(fd,VIDIOCSPICT,pict);
	if (err<0){
#if defined(DEBUG)
		printf("could not set picture properties: %s\n",strerror(errno));
#endif
		return 0;
	}
	return 1;
}
//#endif

int try_size(int fd, int width, int height)
{
	struct video_window win;
	int err;
	memset(&win,0,sizeof(win));
	/*set picture size */
	win.x=win.y=0;
	win.width=width;
	win.height=height;
	win.flags=0;
	win.clips=NULL;
	win.clipcount=0;

#if defined(DEBUG)
	printf("trying to set capture size to %ix%i\n", width,height);
#endif

	err=ioctl(fd,VIDIOCSWIN,&win);
	if (err<0){
#if defined(DEBUG)
		printf("could not set window size: %s\n",strerror(errno));
#endif
		return 0;
	}

	err=ioctl(fd, VIDIOCGWIN, &win);
	if (err<0){
#if defined(DEBUG)
		printf("could not get window size: %s\n",strerror(errno));
#endif
		return 0;
	}
	if (width!=(int)win.width || height!=(int)win.height){
#if defined(DEBUG)
		printf("capture size is not what we expected: asked for %ix%i and get %ix%i\n",
				width, height, win.width, win.height);
#endif
		return 0;
	}

#if defined(DEBUG)
	printf("capture size set to %ix%i\n", width,height);
#endif

	return 1;
}

int do_mmap(int fd, struct video_mbuf * pvmbuf, char ** pbuf)
{
	int err;

	memset((void*)pvmbuf,0,sizeof(*pvmbuf));
	/* try to get mmap properties */
	err=ioctl(fd,VIDIOCGMBUF,pvmbuf);
	if (err<0){
		printf("could not get mmap properties: %s\n",strerror(errno));
		return 0;
	}

	*pbuf = (char*)mmap(NULL,pvmbuf->size,PROT_READ,MAP_PRIVATE,fd,0);
	if (*pbuf == (void*)-1) {
		printf("could not mmap: %s\n",strerror(errno));
		return 0;
	}

	return 1;
}

int opendev(const char *device, int width, int height, int *palette)
{
	int devfd;
	int err = -1;
	struct video_capability vidcap;
	struct video_picture pict;
	devfd = open(device, O_RDWR);
	if (devfd < 0) {
#if defined(DEBUG)
		printf("cannot open %s\n", device);
#endif
		return 0;
	}

#if defined(DEBUG)
	printf("opened %s\n", device);
#endif

	// get device capabilities
#ifndef RUNONPC
	//err=ioctl(devfd, VIDIOC_QUERYCAP, &vidcap);
	err=ioctl(devfd, VIDIOCGCAP, &vidcap);
	if (err!=0)
	{
#if defined(DEBUG)
		printf("cannot get device capabilities: %s\n",strerror(errno));
#endif
		return 0;
	}

#if defined(DEBUG)
	printf("found %s device (maxsize = %ix%i)\n",vidcap.name, vidcap.maxwidth, vidcap.maxheight);
#endif

#endif

	// get picture properties
#ifdef RUNONPC
	printf("Exit checking for properties, because on PC/laptop you won't have a v4l compatible device\n");
	palette = NULL;
	return devfd;
#endif
	err=ioctl(devfd, VIDIOCGPICT, &pict);
	if (err<0)
	{
#if defined(DEBUG)
		printf("could not get picture properties: %s\n",strerror(errno));
#endif
		return 0;
	}

#if defined(DEBUG)
	printf("default picture properties: brightness=%i,hue=%i,colour=%i,contrast=%i,depth=%i, palette=%i\n",
			pict.brightness,pict.hue,pict.colour, pict.contrast,pict.depth, pict.palette);
#endif

	// check whether this format is supported
	if (!try_format(devfd, &pict, DEFAULT_FMT, DEFAULT_LEN)) {
#if defined(DEBUG)
		printf("unsupported video pixel format\n");
#endif
		return 0;
	}

	if (!try_size(devfd, width, height))
	{
#if defined(DEBUG)
		printf("unsupported video window size\n");
#endif
		return 0;
	}

	*palette = pict.palette;

	return devfd;
}

int grab(int devfd, int width, int height, int palette,unsigned char* buffer)
{
	int done = 0;
	int j = 0;
	char *vbuf = NULL, *ptr = NULL;
	int err_ioctl;


	struct video_mbuf vmbuf;
	struct video_mmap vmmap;

	int frame_id = 0;
	long start_ms = 0;
	int frame_size;

	if (!devfd)
	{
#if defined(DEBUG)
		printf("video device not open\n");
#endif
		return NULL;
	}

	if (!do_mmap(devfd, &vmbuf, &vbuf)) {
#if defined(DEBUG)
		printf("cannot mmap\n");
#endif
	}

	frame_size = vmbuf.size;

#if defined(DEBUG)
	printf("frame size: %d\n", frame_size);
#endif

	frame_id = 0;

#if defined(DEBUG)
	printf("start grabbing the image\n");
#endif

	/* start to grab */
	vmmap.height = height;
	vmmap.width = width;
	vmmap.format = palette;

	for (j = 0; j < vmbuf.frames; j++) {
		vmmap.frame = j;

#if defined(DEBUG)
		printf("in frame loop\n");
#endif

		ioctl(devfd, VIDIOCMCAPTURE, &vmmap);
	}

	/* capture */
	start_ms = get_cur_ms();

#if defined(DEBUG)
	printf("capturing the image\n");
#endif

	do {
		err_ioctl = ioctl(devfd, VIDIOCSYNC, &frame_id);

#if defined(DEBUG)
		printf("ioctl error = %d \n",err_ioctl);
#endif

		while (err_ioctl < 0 &&
				(errno == EAGAIN || errno == EINTR));

		ptr = vbuf + vmbuf.offsets[frame_id];

#if defined(DEBUG)
		printf("got frame: ms = %lu\n", get_cur_ms()-start_ms);
#endif

		memcpy(buffer, ptr, frame_size);

		done = 1;

	} while (!done);

	return 0;
}
