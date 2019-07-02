/*
 * Leopard Imaging 2016-06-14
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <linux/usb/video.h>
#include <errno.h>
#include <iconv.h>
#include <linux/uvcvideo.h>


// define test register address
#define TEST_REG_ADDR   (0x3012)  


// define the Leopard Imaging USB3.0 camera
// uvc extension id
#define CY_FX_UVC_XU_REG_RW (0x0e00)

// define the value array for storage
// the setting and getting configuration value
unsigned char value[64] = {0}; 

// define the xu query struct
struct uvc_xu_control_query xu_query = 
{
	.unit		= 3, //has to be unit 3
	.selector	= 1, //TD      
	.query		= UVC_SET_CUR,
	.size		= 4, //TD
	.data		= value,
};

// open the /dev/video* uvc camera device
int open_v4l2_device(char *device_name)
{
	int v4l2_dev;
	char *dev_node;

	if(device_name == NULL)
		return -5;

	dev_node = (char *)malloc(5 + strlen(device_name) + 1);
	if(!dev_node)
		return 0;

	sprintf(dev_node, "/dev/%s", device_name);
	v4l2_dev = open(dev_node, 0);
	free(dev_node);

	return v4l2_dev;
}

// hanle the error for opening the device
void error_handle()
{
	int res = errno;

	const char *err;
	switch(res) 
	{
		case ENOENT:	err = "Extension unit or control not found"; break;
		case ENOBUFS:	err = "Buffer size does not match control size"; break;
		case EINVAL:	err = "Invalid request code"; break;
		case EBADRQC:	err = "Request not supported by control"; break;
		default:		err = strerror(res); break;
	}

	printf("failed %s. (System code: %d) \n", err, res);

	return ;
}

// write the sensor register value
void SensorRegWrite(int fd, int regAddr, int regVal)
{
	xu_query.selector = CY_FX_UVC_XU_REG_RW >> 8;
	xu_query.query = UVC_SET_CUR;
	xu_query.size = 5;

	// setting the read configuration
	value[0] = 1; // indicate for write
	value[1] = (regAddr >> 8) & 0xff;
	value[2] = regAddr & 0xff;
	value[3] = (regVal >> 8) & 0xff;
	value[4] = regVal & 0xff;
	if(ioctl(fd, UVCIOC_CTRL_QUERY, &xu_query) != 0) 
		error_handle();
}


// read the sensor register value
int SensorRegRead(int fd, int regAddr)
{
	int regVal = 0;

	xu_query.selector = CY_FX_UVC_XU_REG_RW >> 8;
	xu_query.query = UVC_SET_CUR;
	xu_query.size = 5;

	// setting the read configuration
	value[0] = 0; // indicate for read
	value[1] = (regAddr >> 8) & 0xff;
	value[2] = regAddr & 0xff;
	if(ioctl(fd, UVCIOC_CTRL_QUERY, &xu_query) != 0) 
		error_handle();
	

	// getting the value
	xu_query.query = UVC_GET_CUR;
	value[0] = 0;
	value[3] = 0;
	value[4] = 0;
	if(ioctl(fd, UVCIOC_CTRL_QUERY, &xu_query) != 0) 
		error_handle();

//	sleep(1);

	regVal = (value[3] << 8) + value[4];
	printf("REG[0x%x] = 0x%x\n", regAddr, regVal);

	return regVal;
}


void main()
{
	int v4l2_dev;
	int regval;

	v4l2_dev = open_v4l2_device("video0");
	if(v4l2_dev < 0)
	{
		printf("open camera failed,err code:%d\n\r",v4l2_dev);
		return ;
	}

	regval = SensorRegRead(v4l2_dev, TEST_REG_ADDR);
	regval += 1;
	sleep(1);
	SensorRegWrite(v4l2_dev, TEST_REG_ADDR, regval);
	sleep(1);
	SensorRegRead(v4l2_dev, TEST_REG_ADDR);

	close(v4l2_dev);	

	return;
}


