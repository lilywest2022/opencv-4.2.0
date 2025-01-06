#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h> /* getopt_long() */
#include <fcntl.h> /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <dlfcn.h>


#include "opencv2/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <linux/videodev2.h>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#define CV_YUV2BGR_YUYV_ON 0
#define CV_NV12_ON 0
#define CV_RGB_ON 1
#define CV_RAW_ON 0
struct buffer {
        void *start;
        size_t length;
        struct v4l2_buffer v4l2_buf;
};



/*********************ADD IIC Start*****************/
#define I2C_M_WT				0x0000	/* write data, from master to slave */
#define I2C_M_RD				0x0001	/* read data, from slave to master */
#define I2C_ADDR (0x52>>1)

#define I2C_ADDR_16BIT  1  // 1:16bit i2c_addr 0:8bit i2c_addr
#define I2C_RETRIES         0x0701  
#define I2C_TIMEOUT         0x0702  
#define I2C_SLAVE           0x0703  
#define I2C_SLAVE_FORCE     0x0706  
#define I2C_TENBIT          0x0704  
#define I2C_FUNCS           0x0705  
#define I2C_RDWR            0x0707   /*Combined R/W transfer (one STOP only)  */ 
#define I2C_PEC             0x0708   /* != 0 to use PEC with SMBus            */ 
#define I2C_SMBUS           0x0720  /*SMBus transfer                         */ 
#define I2C_SMBUS_BYTE_DATA	    2 
#define I2C_SMBUS_READ				1
#define I2C_SMBUS_WRITE			0
#define I2C_SMBUS_BLOCK_DATA	    5
#define I2C_SMBUS_I2C_BLOCK_DATA	8
#define I2C_SLAVE_FORCE            0x0706
#define I2C_SMBUS_BLOCK_MAX	32


// exact-width types
typedef char			s8;
typedef short			s16;
typedef int				s32;
typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	u32;
typedef float			f32;
typedef double			f64;


union i2c_smbus_data {
	unsigned char byte;
	unsigned short word;
	unsigned char block[I2C_SMBUS_BLOCK_MAX + 2];
};

struct i2c_smbus_ioctl_data {
	char read_write;
	unsigned char command;
	int size;
	union i2c_smbus_data *data;
};

 struct i2c_msg {
	u16 addr;	/* slave address			*/
	u16 flags;
	u16 len;		/* msg length				*/
	u8 *buf;		/* pointer to msg data			*/
} ;

/* In ./include/linux/i2c-dev.h */
struct i2c_rdwr_ioctl_data {   
 /* pointers to i2c_msgs                 */ 
   struct i2c_msg  *msgs;   
 /* number of i2c_msgs                   */
   u32 nmsgs;
};
/*********************ADD IIC End*****************/

#define BUFFER_COUNT 4
#define FMT_NUM_PLANES 1
#define CLEAR(x) memset(&(x), 0, sizeof(x))

#define DBG(...) do { if(!silent) printf(__VA_ARGS__); } while(0)
#define ERR(...) do { fprintf(stderr, __VA_ARGS__); } while (0)

static unsigned long get_time(void);
static int fd = -1;
FILE *fp=NULL;
static unsigned int n_buffers;
struct buffer *buffers;
static int silent=0;

static char dev_name[255]="/dev/video0";
static int width = 1920;
static int height = 1080;
static enum v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
#if CV_YUV2BGR_YUYV_ON
static int format = V4L2_PIX_FMT_YUYV;
#endif

#if CV_NV12_ON
static int format = V4L2_PIX_FMT_NV12;
#endif

#if CV_RGB_ON
static int format = V4L2_PIX_FMT_RGB24;
#endif
 #if CV_RAW_ON
static int format = V4L2_PIX_FMT_SBGGR10;
 #endif
static void errno_exit(const char *s)
{
	ERR("%s error %d, %s\n", s, errno, strerror(errno));
	exit(EXIT_FAILURE);
}



/************ADD IIC Start**************/



int Standard_i2c_open(int index)
{
	char dev_name[32];

	sprintf(dev_name, "/dev/i2c-%d", index);

	return open(dev_name, O_RDWR);
}




int write_16bit_i2c(int bus_index, int device_addr/*8bits*/, u32 register_addr, unsigned char *pBuffer)
{
 	struct i2c_rdwr_ioctl_data e2prom_data;
	int i2c_dev_fd = -1;
	char reg_buf[3]={0,0,0};
	int ret;
	/**/
	i2c_dev_fd = Standard_i2c_open(bus_index);
	if(i2c_dev_fd < 0)
	{
		printf("standard_i2c_read Standard_i2c_open Failed\n");
		goto _OVER_;
	}
	
	ioctl(i2c_dev_fd,I2C_TIMEOUT,1);
	ioctl(i2c_dev_fd,I2C_RETRIES,2);

	e2prom_data.nmsgs=1; 
	e2prom_data.msgs=(struct i2c_msg*)malloc(e2prom_data.nmsgs*sizeof(struct i2c_msg));
	if(!e2prom_data.msgs)
	{
		 perror("malloc error");
		 goto _OVER_;
	}

	
	reg_buf[0] = (register_addr >> 8) & 0xFF; 
	reg_buf[1] = register_addr & 0xFF ;       
	reg_buf[2] =  pBuffer[0] ; 
	
	
	(e2prom_data.msgs[0]).len=3; 
	(e2prom_data.msgs[0]).addr=device_addr >> 1; 
	(e2prom_data.msgs[0]).flags=I2C_M_WT; //write
	(e2prom_data.msgs[0]).buf =(u8*) reg_buf;
	
	ret=ioctl(i2c_dev_fd,I2C_RDWR,(unsigned long)&e2prom_data);
	if(ret<0)
	{
	     perror("ioctl error2");
		 goto _OVER_;
	}
	//printf("** %x ** \n",pBuffer[0]);
		 

	_OVER_:
	if(i2c_dev_fd >= 0)
	{
		close(i2c_dev_fd);
	}
	if(e2prom_data.msgs != NULL)
	{
		free(e2prom_data.msgs);
	}

}


int read_16bit_i2c(int bus_index, int  device_addr/*8bits*/, u32 register_addr, unsigned char *pBuffer)
{
	u32 x;
	
	struct i2c_rdwr_ioctl_data e2prom_data;
	int i2c_dev_fd = -1;
	char reg_buf[2]={0,0},buf[2]={0,0};
	int ret;
	/**/
	i2c_dev_fd = Standard_i2c_open(bus_index);
	if(i2c_dev_fd < 0)
	{
		printf("standard_i2c_read Standard_i2c_open Failed\n");
		goto _OVER_;
	}
	ioctl(i2c_dev_fd,I2C_TIMEOUT,1);
	ioctl(i2c_dev_fd,I2C_RETRIES,2);

	e2prom_data.nmsgs=2; 
	e2prom_data.msgs=(struct i2c_msg*)malloc(e2prom_data.nmsgs*sizeof(struct i2c_msg));
	if(!e2prom_data.msgs)
	{
		 perror("malloc error");
		 goto _OVER_;
	}

	reg_buf[0] = (register_addr >> 8) & 0xFF; 
	reg_buf[1] = register_addr & 0xFF ;       

	
	e2prom_data.nmsgs=2;
	(e2prom_data.msgs[0]).len=2; 
	(e2prom_data.msgs[0]).addr=device_addr >>1 ; 
	(e2prom_data.msgs[0]).flags=I2C_M_WT; //write
	(e2prom_data.msgs[0]).buf = (u8*)reg_buf;
	
	(e2prom_data.msgs[1]).len=1; 
	(e2prom_data.msgs[1]).addr=device_addr >> 1 ;
	(e2prom_data.msgs[1]).flags=I2C_M_RD;//read
	(e2prom_data.msgs[1]).buf=(u8*)buf;
	
	ret=ioctl(i2c_dev_fd,I2C_RDWR,(unsigned long)&e2prom_data);
	if(ret<0)
	{
	     perror("ioctl error2");
		 goto _OVER_;
	}
	printf("***0x%x***\n",(e2prom_data.msgs[1]).buf[0]);
		 
	pBuffer[0] = (e2prom_data.msgs[1]).buf[0];

	_OVER_:
	if(i2c_dev_fd >= 0)
	{
		close(i2c_dev_fd);
	}
	if(e2prom_data.msgs != NULL)
	{
		free(e2prom_data.msgs);
	}
	
}


/************ADD IIC End****************/




static int xioctl(int fh, int request, void *arg)
{
	int r;
	do {
			r = ioctl(fh, request, arg);
	} while (-1 == r && EINTR == errno);
	return r;
}

static void open_device(void)
{
    fd = open(dev_name, O_RDWR /* required */ /*| O_NONBLOCK*/, 0);

    if (-1 == fd) {
        ERR("Cannot open '%s': %d, %s\n",
                    dev_name, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

static int get_camera_device_fmt()
{
	printf("------------------get camera format------------\n");
	system("export DISPLAY=:0;xhost local:gedit;");
	//system("xhost local:gedit");
	system("v4l2-ctl  --list-formats -d /dev/video0");
		int ret;
	    struct v4l2_capability cap;
		ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);//查看设备功能
		if (ret < 0)
		{
			perror("requre VIDIOC_QUERYCAP fialed! \n");
			return -1;
		}
		printf("driver:%s\n",cap.driver);
		printf("card:%s\n",cap.card);
		printf("bus_info:%s\n",cap.bus_info);
		printf("version:%d\n",cap.version);
		printf("capabilities:%x\n",cap.capabilities);
				
		if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE)
		{
			printf("Device %s: supports capture.\n",dev_name);
		}
	
		if ((cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING)
		{
			printf("Device %s: supports streaming.\n",dev_name);
		}

}


static void init_display_buf(int buffer_size, int width, int height)
{
	cv::namedWindow("V4L2+OpenCV (MAX9295A to MAX96712)");
	cv::resizeWindow("V4L2+OpenCV (MAX9295A to MAX96712)", 640, 480);    //# 设置长和宽
		//cv::namedWindow("4L2+OpenCV (MAX9295A to MAX96712)", 0);  //# 0可调大小，注意：窗口名必须imshow里面的一窗口名一直
}


static void init_mmap(void)
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count = BUFFER_COUNT;
	req.type = buf_type;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
				ERR("%s does not support "
								"memory mapping\n", dev_name);
				exit(EXIT_FAILURE);
		} else {
				errno_exit("VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2) {
		ERR("Insufficient buffer memory on %s\n",
						dev_name);
		exit(EXIT_FAILURE);
	}

	buffers = (struct buffer*)calloc(req.count, sizeof(*buffers));

	if (!buffers) {
		ERR("Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		struct v4l2_buffer buf;
		struct v4l2_plane planes[FMT_NUM_PLANES];
		CLEAR(buf);
		CLEAR(planes);

		buf.type = buf_type;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffers;

		if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
			buf.m.planes = planes;
			buf.length = FMT_NUM_PLANES;
		}

		if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
			errno_exit("VIDIOC_QUERYBUF");

		if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
			buffers[n_buffers].length = buf.m.planes[0].length;
			buffers[n_buffers].start =
			mmap(NULL /* start anywhere */,
					buf.m.planes[0].length,
					PROT_READ | PROT_WRITE /* required */,
					MAP_SHARED /* recommended */,
					fd, buf.m.planes[0].m.mem_offset);
		} else {
			buffers[n_buffers].length = buf.length;
			buffers[n_buffers].start =
			mmap(NULL /* start anywhere */,
					buf.length,
					PROT_READ | PROT_WRITE /* required */,
					MAP_SHARED /* recommended */,
					fd, buf.m.offset);
		}

		if (MAP_FAILED == buffers[n_buffers].start)
				errno_exit("mmap");
	}
}

static int init_device()
{
    struct v4l2_capability cap;
    struct v4l2_format fmt;

    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
            if (EINVAL == errno) {
                    ERR("%s is no V4L2 device\n",
                                dev_name);
                    exit(EXIT_FAILURE);
            } else {
                    errno_exit("VIDIOC_QUERYCAP");
            }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) &&
            !(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)) {
        ERR("%s is not a video capture device, capabilities: %x\n",
                        dev_name, cap.capabilities);
            exit(EXIT_FAILURE);
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
            ERR("%s does not support streaming i/o\n",
                dev_name);
            exit(EXIT_FAILURE);
    }

    if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
        buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    else if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)
        buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;

    CLEAR(fmt);
    fmt.type = buf_type;
    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;
    fmt.fmt.pix.pixelformat = format;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
            errno_exit("VIDIOC_S_FMT");

	if(-1 == xioctl(fd, VIDIOC_G_FMT, &fmt)){//得到图片格式
        perror("set format failed!");
        return -1;
    }
 
    printf("fmt.type:\t\t%d\n",fmt.type);
    printf("pix.pixelformat:\t%c%c%c%c\n", \
            fmt.fmt.pix.pixelformat & 0xFF,\
            (fmt.fmt.pix.pixelformat >> 8) & 0xFF, \
            (fmt.fmt.pix.pixelformat >> 16) & 0xFF,\
            (fmt.fmt.pix.pixelformat >> 24) & 0xFF);
    printf("pix.width:\t\t%d\n",fmt.fmt.pix.width);
    printf("pix.height:\t\t%d\n",fmt.fmt.pix.height);
    printf("pix.field:\t\t%d\n",fmt.fmt.pix.field);

    init_mmap();

    //init_display_buf(fmt.fmt.pix.sizeimage, width, height);
	//init_display_buf(fmt.fmt.pix.sizeimage, 640, 480);
}


static void start_capturing(void)
{
	unsigned int i;
	enum v4l2_buf_type type;

	for (i = 0; i < n_buffers; ++i) {
			struct v4l2_buffer buf;

			CLEAR(buf);
			buf.type = buf_type;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.index = i;

			if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
				struct v4l2_plane planes[FMT_NUM_PLANES];

				buf.m.planes = planes;
				buf.length = FMT_NUM_PLANES;
			}
			if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
					errno_exit("VIDIOC_QBUF");
	}
	type = buf_type;
	if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
			errno_exit("VIDIOC_STREAMON");
}


static void process_buffer(struct buffer* buff, int size,unsigned long t)
{
	double fps;
	int dts[2];
	char string[10];  // 帧率字符串
	unsigned char imgae_rgb[800 * 600 * 3] = {'\0'};  // 可以将RGB图像数据放进这个数组中。
	unsigned long read_end_time;
	/*--------------------YUYV------------------*/
	#if CV_YUV2BGR_YUYV_ON
	cv::Mat yuvmat(cv::Size(width, height), CV_8UC2, buff->start);
	cv::Mat rgbmat(cv::Size(width, height), CV_8UC3);
	cv::cvtColor(yuvmat, rgbmat,cv::COLOR_YUV2BGR_YUYV);
	read_end_time = get_time();
	fps=1000/(read_end_time - t);
	sprintf(string, "%.2f", fps);      // 帧率保留两位小数
	std::string fpsString("FPS:");
	fpsString += string; 
	//DBG("take time %lu ms    fps = %0.2f\n",read_end_time - t,fps);
	cv::putText(rgbmat, // 图像矩阵
			fpsString,                  // string型文字内容
			cv::Point(5, 20),           // 文字坐标，以左下角为原点
			cv::FONT_HERSHEY_SIMPLEX,   // 字体类型
			0.5, // 字体大小
			cv::Scalar(0, 0, 0));       // 字体颜色 
	cv::imshow("V4L2+OpenCV (MAX9295A to MAX96712)", rgbmat);
	cv::waitKey(1);
	#endif
	/*------------------------------------------*/

	/*--------------------NV12-------------------*/
	#if CV_NV12_ON

	cv::Mat yuvbmat(cv::Size(width, height*3/2), CV_8UC1,buff->start);
	cv::Mat rgbmat(cv::Size(width, height), CV_8UC3);
	cv::Mat rgbmat_out(cv::Size(640, 480), CV_8UC3);//s缩放后的窗口大小

	


	cv::cvtColor(yuvbmat, rgbmat,cv::COLOR_YUV2BGR_NV12);
	cv:resize(rgbmat,rgbmat_out,cv::Size(640,480),0,0,cv::INTER_AREA); //缩放操作





	std::string fpsString("Camera input: 1920x1080");

	cv::putText(rgbmat_out, // 图像矩阵
			fpsString,                  // string型文字内容
			cv::Point(5, 20),           // 文字坐标，以左下角为原点
			cv::FONT_HERSHEY_SIMPLEX,   // 字体类型
			0.5, // 字体大小
			cv::Scalar(0, 0, 255));       // 字体颜色 

	std::string fpsString_out("HDMI output: 640x480");

	cv::putText(rgbmat_out, // 图像矩阵
			fpsString_out,                  // string型文字内容
			cv::Point(5, 40),           // 文字坐标，以左下角为原点
			cv::FONT_HERSHEY_SIMPLEX,   // 字体类型
			0.5, // 字体大小
			cv::Scalar(0, 0, 255));       // 字体颜色 

	// read_end_time = get_time();
	// fps=1000/(read_end_time - t);
	// sprintf(string, "%.2f", fps);      // 帧率保留两位小数
	// std::string fpsString_fps("FPS:");
	// fpsString_fps += string; 
	// DBG("take time %lu ms    fps = %0.2f\n",read_end_time - t,fps);
	// cv::putText(rgbmat_out, // 图像矩阵
	// 		fpsString_fps,                  // string型文字内容
	// 		cv::Point(5, 60),           // 文字坐标，以左下角为原点
	// 		cv::FONT_HERSHEY_SIMPLEX,   // 字体类型
	// 		0.5, // 字体大小
	// 		cv::Scalar(0, 0, 255));       // 字体颜色 
	//cv::moveWindow("V4L2+OpenCV (MAX9295A to MAX96712)",100,100);

	cv::imshow("V4L2+OpenCV (MAX9295A to MAX96712)", rgbmat_out);
	cv::waitKey(1);
	#endif
	/*-------------------------------------------*/






	/*--------------------RAW to RGB-------------------*/
	#if CV_RAW_ON
	cv::Mat rawmat(cv::Size(width, height), CV_16SC1,buff->start);
	cv::Mat rgbmat(cv::Size(width, height), CV_8UC3);
	cv::cvtColor(rawmat, rgbmat,cv::COLOR_BayerRG2BGR);
	cv::imshow("V4L2+OpenCV (MAX9295A to MAX96712)", rgbmat);
	cv::waitKey(1);
	#endif
	/*-------------------------------------------*/
		/*--------------------RGB-------------------*/
	#if CV_RGB_ON
	cv::Mat rgb24mat(cv::Size(width, height), CV_8UC3,buff->start);
	/* memcpy(imgae_rgb, buff->start, 800*600*3*sizeof(unsigned char));
	for(int i;i<800*600*3;i++)
	{
		printf("%x",imgae_rgb[i]);
	}; */

	//cv::cvtColor(rgb24mat, rgbmat,cv::COLOR_RGB2GRAY);
	cv::imshow("V4L2+OpenCV (MAX9295A to MAX96712)", rgb24mat);

	cv::waitKey(1);
	#endif
	/*-------------------------------------------*/
}


static int read_frame(unsigned long t)
{
	struct v4l2_buffer buf;
	int i, bytesused;
	unsigned long read_start_time, read_end_time;
	CLEAR(buf);

	buf.type = buf_type;
			buf.memory = V4L2_MEMORY_MMAP;

	if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
			struct v4l2_plane planes[FMT_NUM_PLANES];
			buf.m.planes = planes;
			buf.length = FMT_NUM_PLANES;
	}

	if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf))
			errno_exit("VIDIOC_DQBUF");

	i = buf.index;

	if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type)
			bytesused = buf.m.planes[0].bytesused;
	else
			bytesused = buf.bytesused;
				read_start_time = get_time();
			process_buffer(&(buffers[i]), bytesused,read_start_time);
	//DBG("bytesused %d\n", bytesused);

	if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
			errno_exit("VIDIOC_QBUF");

	return 1;
}

static unsigned long get_time(void)
{
	struct timeval ts;
	gettimeofday(&ts, NULL);
	return (ts.tv_sec * 1000 + ts.tv_usec / 1000);
}


static void mainloop(void)
{
	unsigned int count = 1;

	unsigned char value[4];
	value[0]=0x04;
	value[1]=0x84;
	while (1) {
		//t = (double)cv::getTickCount();
		//DBG("No.%d\n", count);        //Display the current image frame number
		

		read_frame(0);

	}
	DBG("\nREAD AND SAVE DONE!\n");
}

int main(int argc, char *argv[])
{

    open_device();
	get_camera_device_fmt();
    init_device();
    start_capturing();
    mainloop();

    return 0;
}
