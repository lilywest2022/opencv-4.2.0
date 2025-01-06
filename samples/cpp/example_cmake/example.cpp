
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


#if 1
#define CV_NV12_ON 1

struct buffer {
		
        void *start;
        size_t length;
        struct v4l2_buffer v4l2_buf;
};

struct video_ch {
	int fd;
	struct buffer *buffers;
};

pthread_mutex_t mutex; //互斥锁类型
pthread_cond_t cond;
#define BUFFER_COUNT 5
#define FMT_NUM_PLANES 1
#define CLEAR(x) memset(&(x), 0, sizeof(x))

#define DBG(...) do { if(!silent) printf(__VA_ARGS__); } while(0)
#define ERR(...) do { fprintf(stderr, __VA_ARGS__); } while (0)

 unsigned long get_time(void);


 int fd = -1;
//int ch=4;


FILE *fp=NULL;
 unsigned int n_buffers;

struct video_ch ch[]={};
 int silent=0;

 char dev_name[255]="\n";
 int width = 1920*4;
 int height = 1080;
 enum v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;


 int format = V4L2_PIX_FMT_NV12;


 void errno_exit(const char *s)
{
	ERR("%s error %d, %s\n", s, errno, strerror(errno));
	exit(EXIT_FAILURE);
}



 int xioctl(int fh, int request, void *arg)
{
	int r;
	do {
			r = ioctl(fh, request, arg);
	} while (-1 == r && EINTR == errno);
	return r;
}

 void open_device(int video_id)
{

	char dev_name[32];

	sprintf(dev_name, "/dev/video%d", video_id);

	ch[video_id].fd = open(dev_name, O_RDWR /* required */ /*| O_NONBLOCK*/, 0);

	if (-1 == ch[video_id].fd) {
		ERR("Cannot open '%s': %d, %s\n",
					dev_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

}
 int get_camera_device_fmt(int video_id)
{
		int ret;
	    struct v4l2_capability cap;
		ret = ioctl(ch[video_id].fd, VIDIOC_QUERYCAP, &cap);//查看设备功能
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


 void init_display_buf(int buffer_size, int width, int height,int video_id)
{
	char string[10]; 
	sprintf(string, "%d", video_id); 
	std::string fpsString_fps("V4L2+OpenCV (MAX9295A to MAX96712)_vc");
	fpsString_fps += string; 

	cv::resizeWindow(fpsString_fps, 640, 480);    //# 设置长和宽

}


 void init_mmap(int video_id)
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count = BUFFER_COUNT;
	req.type = buf_type;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(ch[video_id].fd, VIDIOC_REQBUFS, &req)) {
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

	ch[video_id].buffers = (struct buffer*)calloc(req.count, sizeof(*(ch[video_id].buffers))*3);

	if (!ch[video_id].buffers) {
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

		if (-1 == xioctl(ch[video_id].fd, VIDIOC_QUERYBUF, &buf))
			errno_exit("VIDIOC_QUERYBUF");

		if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
			ch[video_id].buffers[n_buffers].length = buf.m.planes[0].length;
			ch[video_id].buffers[n_buffers].start =
			mmap(NULL /* start anywhere */,
					buf.m.planes[0].length,
					PROT_READ | PROT_WRITE /* required */,
					MAP_SHARED /* recommended */,
					ch[video_id].fd, buf.m.planes[0].m.mem_offset);
		} else {
			ch[video_id].buffers[n_buffers].length = buf.length;
			ch[video_id].buffers[n_buffers].start =
			mmap(NULL /* start anywhere */,
					buf.length,
					PROT_READ | PROT_WRITE /* required */,
					MAP_SHARED /* recommended */,
					ch[video_id].fd, buf.m.offset);
		}

		if (MAP_FAILED == ch[video_id].buffers[n_buffers].start)
				errno_exit("mmap");
	}
}

 int init_device(int video_id)
{
    struct v4l2_capability cap;
    struct v4l2_format fmt;

    if (-1 == xioctl(ch[video_id].fd, VIDIOC_QUERYCAP, &cap)) {
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

    if (-1 == xioctl(ch[video_id].fd, VIDIOC_S_FMT, &fmt))
            errno_exit("VIDIOC_S_FMT");

	if(-1 == xioctl(ch[video_id].fd, VIDIOC_G_FMT, &fmt)){//得到图片格式
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

    init_mmap(video_id);

    init_display_buf(fmt.fmt.pix.sizeimage, width, height,video_id);
}


 void start_capturing(int video_id)
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
		if (-1 == xioctl(ch[video_id].fd, VIDIOC_QBUF, &buf))
					errno_exit("VIDIOC_QBUF");
	}
	type = buf_type;
	if (-1 == xioctl(ch[video_id].fd, VIDIOC_STREAMON, &type))
			errno_exit("VIDIOC_STREAMON");
}


 void process_buffer(struct buffer* buff, int size,unsigned long t,int video_id)
{
	double fps;
	int dts[2];
	char string[10];  // 帧率字符串
	unsigned long read_end_time;

	cv::Mat yuvbmat(cv::Size(width, height*3/2), CV_8UC1,buff->start);
	cv::Mat rgbmat(cv::Size(width, height), CV_8UC3);
	cv::Mat rgbmat_out(cv::Size(2560, 480), CV_8UC3);//s缩放后的窗口大小

	cv::cvtColor(yuvbmat, rgbmat,cv::COLOR_YUV2BGR_NV12);
	cv:resize(rgbmat,rgbmat_out,cv::Size(2560,1080),0,0,cv::INTER_AREA); //缩放操作

	//std::string fpsString("Camera input: 1920x1080");

	//cv::putText(rgbmat_out, // 图像矩阵
	//		fpsString,                  // string型文字内容
	//		cv::Point(5, 20),           // 文字坐标，以左下角为原点
	//		cv::FONT_HERSHEY_SIMPLEX,   // 字体类型
	//		0.5, // 字体大小
	//		cv::Scalar(0, 0, 255));       // 字体颜色 

	//std::string fpsString_out("HDMI output: 640x480");

	//cv::putText(rgbmat_out, // 图像矩阵
	//		fpsString_out,                  // string型文字内容
	//		cv::Point(5, 40),           // 文字坐标，以左下角为原点
	//		cv::FONT_HERSHEY_SIMPLEX,   // 字体类型
	//		0.5, // 字体大小
	//		cv::Scalar(0, 0, 255));       // 字体颜色 

	sprintf(string, "%d", video_id);      // 帧率保留两位小数
	//std::string fpsString_fps("V4L2+OpenCV (MAX9295A to MAX96712)_vc");
	std::string fpsString_fps("video0_同步聚合测试");
	fpsString_fps += string; 
	if(video_id==0){
		cv::moveWindow(fpsString_fps, 0, 0);
		cv::imshow(fpsString_fps, rgbmat_out);
	}
	else if(video_id==1){
		cv::moveWindow(fpsString_fps, 640+100, 0);
		cv::imshow(fpsString_fps, rgbmat_out);
	}
	else if(video_id==2){
		cv::moveWindow(fpsString_fps, 0, 480+100);
		cv::imshow(fpsString_fps, rgbmat_out);
	}
	else{
		cv::moveWindow(fpsString_fps, 640+100, 480+100);
		cv::imshow(fpsString_fps, rgbmat_out);
	}
	cv::waitKey(1);

}

 int read_frame(unsigned long t,int video_id)
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
		fd_set fds;
        FD_ZERO(&fds);
        FD_SET(ch[video_id].fd, &fds);
        struct timeval tv = {0};
        tv.tv_sec = 2;
        int r = select(ch[video_id].fd+ 1, &fds, NULL, NULL, &tv);
        if (r == -1) {
            errno_exit("Waiting for Frame");
           // break;
        }

			if (-1 == xioctl(ch[video_id].fd, VIDIOC_DQBUF, &buf))
					errno_exit("VIDIOC_DQBUF");

			i = buf.index;

			if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type)
					bytesused = buf.m.planes[0].bytesused;
			else
					bytesused = buf.bytesused;
					pthread_mutex_lock(&mutex);   /*互斥锁上锁*/
					process_buffer(&(ch[video_id].buffers[i]), bytesused,read_start_time,video_id);
					pthread_mutex_unlock(&mutex); /*互斥锁解锁*/
						pthread_cond_broadcast(&cond);/*广播方式唤醒休眠的线程*/
	
					//}
			//DBG("bytesused %d\n", bytesused);
			if (-1 == xioctl(ch[video_id].fd, VIDIOC_QBUF, &buf))
					errno_exit("VIDIOC_QBUF");
	return 1;
}

 unsigned long get_time(void)
{
	struct timeval ts;
	gettimeofday(&ts, NULL);
	return (ts.tv_sec * 1000 + ts.tv_usec / 1000);
}


 void mainloop(int video_id)
{
	int cur_time;
	while (1) {
		//cur_time=get_time();
		//printf("video_id:%d  cur_time %d\n",video_id, cur_time);
		read_frame(0,video_id);
	
	}
}




#endif

