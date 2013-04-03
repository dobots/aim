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

#ifdef RUNONPC
	#include <libv4l1.h>
#else
#include <linux/videodev.h>
#endif

#include "libcam.h"


/*Function used to write the value of parameter on the i2c bus*/
static int WriteWord(char file,unsigned char offset, unsigned short data)
{
	__u8 buf[2];
	int err;
	buf[0] = offset;
	buf[1] = data;

	err =write(file, buf, 2);
	//printf("err= %d ",err);  // if device not present: err = -1
				 // if device present and data send = 2
	return err;
}

/* function to initialise the camera */
int cam_init(void)
{
	int file,err=0;
	int adapter_nr = 0; /* this needs to be the same as in modprobe(check on the core) */
	char filename[20];

	snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
	file = open(filename, O_RDWR);

	if (file < 0) {
    #if defined(DEBUG)
		printf ("error occurred while creating adapter to i2c \n");
	#endif
		return -1;
	}
	int addr = 0x21; /* The I2C address */

	if (ioctl(file, I2C_SLAVE, addr) < 0) {
    #if defined(DEBUG)
		printf ("cannot find camera\n");
	#endif
   		return -1;   // cannot find the camera

		/*printf ("error occurred while talking to i2c slave address \n");
    		exit(1);*/
  	}
	else
	{
    #if defined(DEBUG)
		printf("initializing the camera\n");
	#endif

		err = WriteWord(file, 0x12, 0x80); /* reset */
		if(err == -1)
		{
    #if defined(DEBUG)
			printf("camera not found\n");
	#endif
			return -1;
		}
		usleep(100);
		const char *b, *end;
		b = Config7670;
		end = &b[sizeof(Config7670)];
		while (b < end) {
			err = WriteWord(file, b[0], b[1]);
			if(err == -1)
			{
    #if defined(DEBUG)
				printf("camera not found\n");
	#endif
				return -1;
			}
			usleep(100);
			b += 2;
		}
	}
  	return 0;
}

/* function to select the camera */
int cam_select(int cam)
{
	int file, err=0;
	int adapter_nr = 0; /* this needs to be the same as in modprobe(check on the core) */
	char filename[20];

	snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
	file = open(filename, O_RDWR);

	if (file < 0) {
    #if defined(DEBUG)
		printf ("error occurred while creating adapter to i2c\n");
	#endif
		return -1;
	}
	int addr = PORT_EXP_ADDR; /* put address of the port expander */
	if (ioctl(file, I2C_SLAVE_FORCE, addr) < 0) {
    #if defined(DEBUG)
    	printf ("error occurred while talking to port expander\n");
	#endif
    	return -1;
  	}
	else
	{
	    // writeword cannot be used to send the data via i2c to portexapnder. PE accepts data
        // in a different manner from the camera
    #if defined(DEBUG)
		printf("camera %d\n", cam);
	#endif

		err = WriteWord(file,0x03,0x00);
		if(err == -1)
		{
    #if defined(DEBUG)
			printf("error occured when talking to port expander\n");
	#endif
			return -1;
		}
		else
		{

			switch(cam)
			{
                case 1 : WriteWord(file,0x01, 0xfd);    // cam 1 activated   01
                        break;

                case 2 : WriteWord(file,0x01, 0xf7);    // cam 2 activated  04
                        break;

                case 3 : WriteWord(file,0x01, 0xdf);    // cam 3 activated   10
                        break;

                case 4 : WriteWord(file,0x01, 0x7f);    // cam 4 activated   40
			}
        }
	}

	return 0;
}

/* function tested, all cameras are powered down, nothing is detected on the i2c bus*/
int cam_powerdown_all(void)
{
	int file, err=0;
	int adapter_nr = 0; /* this needs to be the same as in modprobe(check on the core) */
	char filename[20];

	snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
	file = open(filename, O_RDWR);
	if (file < 0) {
    #if defined(DEBUG)
		printf ("error occurred while creating adapter to i2c \n");
	#endif
		return -1;
	}
	int addr = PORT_EXP_ADDR; /* put address of the port expander */
	if (ioctl(file, I2C_SLAVE_FORCE, addr) < 0) {
    #if defined(DEBUG)
    		printf ("error occurred while talking to port expander \n");
	#endif
    		return -1;
  	}
	else
	{
		err = WriteWord(file,0x03,0x00);
		if(err == -1)
		{
    #if defined(DEBUG)
			printf("error occured when talking to port expander, cannot powerdown camera\n");
	#endif
			return -1;
		}
    #if defined(DEBUG)
		printf("powering the camera down\n");
	#endif
		err = WriteWord(file,0x01, 0xff );  //00
		if(err == -1)
		{
    #if defined(DEBUG)
			printf("error occurred when talking to port expander, cannot powerdown camera\n");
	#endif
			return -1;
		}
	}

	return 0;

}
int cam_brightness( signed int val)
{
	int file, err=0;
	int adapter_nr = 0; /* this needs to be the same as in modprobe(check on the core) */
	char filename[20];

	snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
	file = open(filename, O_RDWR);
	if (file < 0) {
    #if defined(DEBUG)
		printf ("error occurred while creating adapter to i2c \n");
	#endif
		return -1;
	}
	int addr =  0x21; /* put address of the port expander */
	if (ioctl(file, I2C_SLAVE_FORCE, addr) < 0) {
    #if defined(DEBUG)
    		printf ("error occurred while talking to port expander \n");
    #endif
    		return -1;
  	}
	else
	{
		//calculate the value to be written
		if(val <0)
			val = -(val - 128);
		 #if defined(DEBUG)
		printf("value = %d", val);	
		 #endif
		err = WriteWord(file,0x55,val);
		if(err == -1)
		{
    #if defined(DEBUG)
			printf("error occured when talking to port expander, cannot change brightness\n");
    #endif
			return -1;
		}
    #if defined(DEBUG)
		printf("camera brightness changed\n");
	#endif
	}
	return 0;
}

int cam_frequency( signed int val)
{
	int file, err=0;
	int adapter_nr = 0; /* this needs to be the same as in modprobe(check on the core) */
	char filename[20];

	snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
	file = open(filename, O_RDWR);
	if (file < 0) {
    #if defined(DEBUG)
		printf ("error occurred while creating adapter to i2c \n");
	#endif
		return -1;
	}
	int addr =  0x21; // put address of camera
	if (ioctl(file, I2C_SLAVE_FORCE, addr) < 0) {
    #if defined(DEBUG)
    		printf ("error occurred while talking to port expander \n");
    #endif
    		return -1;
  	}
	else
	{
		//calculate the value to be written

		switch(val)
		{
			case 5: err = WriteWord(file,0x11,0x83);
				err = WriteWord(file,0x6b,0x0a);
				break;
			case 10:err = WriteWord(file,0x11,0x81);
				err = WriteWord(file,0x6b,0x0a);
				break;
			case 20:err = WriteWord(file,0x11,0x80);
				err = WriteWord(file,0x6b,0x0a);
				break;
			case 40:err = WriteWord(file,0x11,0x81);
				err = WriteWord(file,0x6b,0x4a);
				break;
			default: return -1;
		}		


		if(err == -1)
		{
    #if defined(DEBUG)
			printf("error occured when talking to camera, cannot change frequency\n");
    #endif
			return -1;
		}
    #if defined(DEBUG)
		printf("camera brightness changed\n");
	#endif
	}
	return 0;
	
}

/*
int cam_powerup(int cam)
{

	int file;
	int adapter_nr = 0;
	char filename[20];

	snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
	file = open(filename, O_RDWR);
	if (file < 0) {
		printf ("error occurred while creating adapter to i2c \n");
		exit(1);
	}
	int addr = ;
	if (ioctl(file, I2C_SLAVE, addr) < 0) {
    		printf ("error occurred while talking to i2c slave address \n");
    		return 1;
  	}
	else
	{
		printf("Powering the camera up\n");
		WriteWord(file,// value of register of port expander,value);
		system("rmmod blackfin-cam");
		system("modprobe blackfin-cam");
	}

	return 0;

}*/

