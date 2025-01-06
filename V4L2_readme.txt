v4l2预览mipi csi
	apt-get install -y git libopencv-dev cmake libdrm-dev g++ librga-dev

	 gst-launch-1.0 v4l2src device=/dev/video0 io-mode=4 ! queue ! video/x-raw,format=NV12,width=2112,height=1568,framerate=30/1  ! glimagesink


	gst-launch-1.0 v4l2src device=/dev/video0 ! video/x-raw,format=NV12,width=1920,height=1080, framerate=30/1 ! videoconvert ! rkximagesink &
	
	
	gst-launch-1.0 v4l2src device=/dev/video0 ! video/x-raw,format=NV12,width=1920,height=1080, framerate=30/1 ! videoconvert ! rkximagesink
	
	
	
	gst-launch-1.0 v4l2src device=/dev/video0 ! video/x-raw,format=YUYV,width=640,height=480, framerate=30/1 ! videoconvert ! kmssink &
	
	gst-launch-1.0 v4l2src device=/dev/video1 ! video/x-raw,format=NV12,width=640,height=480, framerate=30/1 ! videoconvert ! rkximagesink &
	gst-launch-1.0 v4l2src device=/dev/video0 ! video/x-raw,format=RGB24,width=640,height=480, framerate=30/1 ! videoconvert ! kmssink &
	
	
	gst-launch-1.0 v4l2src device=/dev/video5 ! video/x-raw,format=NV12,width=640,height=480, framerate=30/1 ! videoconvert ! kmssink &
	
	gst-launch-1.0 v4l2src device=/dev/video0 ! video/x-raw,format=YUYV,width=640,height=480, framerate=30/1 ! videoconvert ! kmssink &

 gst-launch-1.0 v4l2src device=/dev/video0 io-mode=4 ! videoconvert ! video/x-raw,format=NV12,width=640,height=480  ! rkximagesink


 gst-launch-1.0 v4l2src device=/dev/video0 io-mode=4 ! queue ! video/x-raw,format=NV12,width=1920,height=1080,framerate=30/1  ! rkximagesink






查找wifi
	nmcli dev wifi
连接wifi
	sudo nmcli d wifi connect "Xiaomi13" password "88888888"
安装NFS
	sudo apt-get install nfs-kernel-server
挂载NFS
	sudo mount -t nfs 192.168.1.50:/nfs /mnt
安装opencv
	sudo dpkg


deb http://security.ubuntu.com/ubuntu/ jammy main 

 strings /lib/aarch64-linux-gnu/libc.so.6 |grep GLIBC_


永久配置DISPLAY环境变量
cd /etc/profile.d
vim x11.h
添加：export DISPLAY=:0


./opencv_example_vc0_input1080p_out640x480&
./opencv_example_vc1_input1080p_out640x480&
./opencv_example_vc2_input1080p_out640x480&
./opencv_example_vc3_input1080p_out640x480&


/home/firefly/Desktop/opencv_example_vc0_input1080p_out640x480;/home/firefly/Desktop/opencv_example_vc1_input1080p_out640x480;


V4L2+opencv运行命令
	nohup sudo -u firefly DISPLAY=:0 ./opencv &



操作v4l2 demo
	git clone https://github.com/T-Firefly/rkisp-v4l2.git
	cd firefly-rkisp-v4l2/v4l2_simple_demo
	make
	
opencv操作摄像头
	https://github.com/T-Firefly/rkisp-v4l2.git

	git clone https://github.com/T-Firefly/rkisp-v4l2.git
	cd rkisp-v4l2/mipi_video_demo/v4l2_simple_demo
	make
	# open /dev/video0
	sudo -u firefly DISPLAY=:0 ./opencv
	
查看拓扑图
	media-ctl -p /dev/media0
	
	media-ctl -d /dev/media0 --set-v4l2 '"rkisp1-isp-subdev":0[fmt:SBGGR10_1X10/4224x3136]'
	
	media-ctl -d /dev/media0 --set-v4l2 '"m00_b_ov13850s":0[fmt:SBGGR10_1X10/4224x3136]'
	media-ctl -d /dev/media0 --set-v4l2 '"rkisp1-isp-subdev":0[fmt:SBGGR10_1X10/2112x1568]'
	
	media-ctl -d /dev/media0 --set-v4l2 '"rkisp1-isp-subdev":2[fmt:YUYV8_2X8/2112x1568]'

	media-ctl -d /dev/media0 --set-v4l2 '"m00_b_ov13850 1-0036":0[fmt:YUYV8_2X8/2112x1568]'
	
	media-ctl --set-v4l2 '"m00_b_ov13850 1-0036":0[fmt:SBGGR10_1X10]'
	
	gst-launch-1.0 v4l2src device=/dev/video0  ! video/x-raw,width=640,height=480,framerate=20/1  ! autovideosink
	
	
	v4l2-ctl --verbose -d /dev/video0 --set-fmt-video=width=1920,height=1080,pixelformat='NV12' --stream-mmap=4 --set-selection=target=crop,flags=0,top=0,left=0,width=1920,height=1080 --stream-to=./out55.yuv&\
	v4l2-ctl --verbose -d /dev/video1 --set-fmt-video=width=1920,height=1080,pixelformat='NV12' --stream-mmap=4 --set-selection=target=crop,flags=0,top=0,left=0,width=1920,height=1080 --stream-to=./out66_640x480.yuv&
	
	
	//抓RAW数据
	v4l2-ctl --verbose -d /dev/video0 --set-fmt-video=width=1920,height=1080,pixelformat='BG10' --stream-mmap=3 --stream-skip=20 --stream-to=./1080p60.raw --stream-count=5 --stream-poll
	v4l2-ctl --verbose -d /dev/video0 --set-fmt-video=width=1920,height=1080,pixelformat='BG10' --stream-mmap=4 --set-selection=target=crop,flags=0,top=0,left=0,width=1920,height=1080 --stream-to=./RAW2.raw
	v4l2-ctl -d /dev/video0  --set-fmt-video=width=1920,height=1080,pixelformat=BG10 --stream-mmap=4 --stream-count=1 --stream-to=./cap1.raw  --stream-skip=2
	
	v4l2-ctl --verbose -d /dev/video0 --set-fmt-video=width=640,height=480,pixelformat='NV12' --stream-mmap=4 --set-selection=target=crop,flags=0,top=0,left=0,width=640,height=480 --stream-to=./outaaa.yuv
	v4l2-ctl --verbose -d /dev/video5 --set-fmt-video=width=640,height=480,pixelformat='NV12' --stream-mmap=4 --set-selection=target=crop,flags=0,top=0,left=0,width=640,height=480 --stream-to=./out44.yuv
	
	v4l2-ctl --verbose -d /dev/video0 --set-fmt-video=width=1920,height=1080,pixelformat='RGB3' --stream-mmap=4 --set-selection=target=crop,flags=0,top=0,left=0,width=1920,height=1080 --stream-to=./aaa.rgb
	
	v4l2-ctl --verbose -d /dev/video5 --set-fmt-video=width=1920,height=1080,pixelformat='NV12' --stream-mmap=4 --set-selection=target=crop,flags=0,top=0,left=0,width=1920,height=1080 --stream-to=./out33.yuv
	
	v4l2-ctl --verbose -d /dev/video0 --set-fmt-video=width=640,height=480,pixelformat='NV12' --stream-mmap=4 --set-selection=target=crop,flags=0,top=0,left=0,width=640,height=480 --stream-to=./GMSL2_D_640x480_30fps_out.yuv
	
	//max96712 节点
	v4l2-ctl --verbose -d /dev/video15 --set-fmt-video=width=1920,height=1080,pixelformat='NV12' --stream-mmap=4 --set-selection=target=crop,flags=0,top=0,left=0,width=1920,height=1080 --stream-to=./out1111.yuv
	
	v4l2-ctl --verbose -d /dev/video15 --set-fmt-video=width=1920,height=1080,pixelformat='NV12' --stream-mmap=4 --set-selection=target=crop,flags=0,top=0,left=0,width=1920,height=1080 --stream-to=./out1.yuv
	
	//RAW非紧凑型   vc0  vc1  vc2  vc3
	echo 0 0 0 0 > /sys/devices/platform/rkcif-mipi-lvds4/compact_test

	//RAW紧凑型
	echo 1 > /sys/devices/platform/rkcif-mipi-lvds4/compact_test

	//抓取VC【0】 通道一帧RAW数据
	v4l2-ctl -d /dev/video0  --set-fmt-video=width=1920,height=1080,pixelformat='BG10' --stream-mmap=4 --stream-count=1 --stream-to=./vc0_1920x1080.raw  --stream-skip=50

	//抓取VC【1】 通道一帧RAW数据
	v4l2-ctl -d /dev/video1  --set-fmt-video=width=1920,height=1080,pixelformat='BG10' --stream-mmap=4 --stream-count=1 --stream-to=./vc1_1920x1080.raw  --stream-skip=50

	//抓取VC【2】 通道一帧RAW数据
	v4l2-ctl -d /dev/video2  --set-fmt-video=width=1920,height=1080,pixelformat='BG10' --stream-mmap=4 --stream-count=1 --stream-to=./vc2_1920x1080.raw  --stream-skip=50


	v4l2-ctl --verbose -d /dev/video0 --set-fmt-video=width=3840,height=1080,pixelformat='NV12' --stream-mmap=4 --set-selection=target=crop,flags=0,top=0,left=0,width=3840,height=1080 --stream-to=./test123.yuv

	ffmpeg -vcodec rawvideo -f rawvideo -pix_fmt bayer_gbrg10 -s 1920*1080 -i RAW1.raw -f image2 -vcodec bmp ./1.bmp
	
	
	v4l2-ctl -d /dev/video0  --set-fmt-video=width=1920,height=1080,pixelformat='RGB3' --stream-mmap=4 --stream-count=1 --stream-to=./aaa.rgb  --stream-skip=50
	
	{0x3612, 0x27},																									
	{0x370a, 0x26},
	{0x372a, 0x00},
	{0x372f, 0x90},
	{0x3801, 0x08},
	{0x3805, 0x97},
	{0x3807, 0x4b},
	{0x3808, 0x08},
	{0x3809, 0x40},
	{0x380a, 0x06},
	{0x380b, 0x20},
	{0x380c, 0x12},
	{0x380d, 0xc0},
	{0x380e, 0x06},
	{0x380f, 0x80},
	{0x3813, 0x02},
	{0x3814, 0x31},
	{0x3815, 0x31},
	{0x3820, 0x02},
	{0x3821, 0x05},
	{0x3836, 0x08},
	{0x3837, 0x02},
	{0x4601, 0x04},
	{0x4603, 0x00},
	{0x4020, 0x00},
	{0x4021, 0xE4},
	{0x4022, 0x07},
	{0x4023, 0x5F},
	{0x4024, 0x08},
	{0x4025, 0x44},
	{0x4026, 0x08},
	{0x4027, 0x47},
	{0x4603, 0x01},
	{0x5401, 0x61},
	{0x5405, 0x40},
	{REG_NULL, 0x00},
	
	mount -t nfs 192.168.1.3:/nfs /mnt
	cd /mnt/rk3399-MAX96712/opencv-4.2.0/samples/cpp/example_cmake/
	cd /mnt/rk3399-MAX96712/ffmedia/ffmedia_release-master_corpro/build
	
	./demo /dev/video0 -o 640x480 -x 0
	./corpro /dev/video0 -o 640x480 -x 0
	./corpro /dev/video0 /dev/video1 -o 640x480 -x 0
	
	
	./corpro /dev/video0 -o 1920x1080 -x 0
	./corpro /dev/video0 -o 1920x1080 -x 0
	
	./corpro /dev/video0 -o 640x480 -d 0
	./corpro /dev/video0 -e h265 -m out1.mp4
	./corpro /dev/video0 -o 2560x1080 -d 0
	
	chown  -R   firefly:firefly   camera_vc_channel.desktop

	
	
* * * * * /root/test.h
* * * * * sleep 5;  /root/test.h
* * * * * sleep 10; /root/test.h
* * * * * sleep 15; /root/test.h
* * * * * sleep 20; /root/test.h
* * * * * sleep 25; /root/test.h
* * * * * sleep 30; /root/test.h
* * * * * sleep 35; /root/test.h
* * * * * sleep 40; /root/test.h
* * * * * sleep 45; /root/test.h
* * * * * sleep 50; /root/test.h
* * * * * sleep 55; /root/test.h

	
	
	
	dmesg | grep ov13850
	
	
	modetest -s 197@196:1920x1080@RG24
	
	gst-launch-1.0 v4l2src device=/dev/video0 ! video/x-raw,format=NV12,width=640,height=480, framerate=30/1 ! videoconvert ! kmssink &
	
	
	gst-launch-1.0 v4l2src ! video/x-raw,width=640,height=480,framerate=20/1  ! autovideosink

	
	
UART

	echo -e '\x01\x02\03' > /dev/ttyS0

	
	
