
#include "example.h"
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h> 



void *video0_show(void *arg)
{
	start_capturing(0);
    mainloop(0);
}

void *video1_show(void *arg)
{
	start_capturing(1);
    mainloop(1);
}

void *video2_show(void *arg)
{
	start_capturing(2);
    mainloop(2);
}

void *video3_show(void *arg)
{	
	start_capturing(3);
    mainloop(3);
}


int main(int argc, char *argv[])
{
	char *dev_names[] = {"/dev/video0", "/dev/video1" ,"/dev/video2","/dev/video3"}; // Add more device names as needed  ,"/dev/video2","/dev/video3"
    int num_cameras = sizeof(dev_names) / sizeof(dev_names[0]);
	pthread_t video0_fd,video1_fd,video2_fd,video3_fd;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

	int video_num=5;
	printf("num_cameras   =%d\n",num_cameras);
	//for(int i=0;i<num_cameras;i++){
		open_device(0);
		get_camera_device_fmt(0);
		init_device(0);
        
	//}
    // 设置新线程的栈大小
    //size_t stack_size = 1024 * 1024*1024; // 1MB
    //pthread_attr_setstacksize(&attr, stack_size);

	//if((num_cameras+1)>1)
    	pthread_create(&video0_fd,NULL,video0_show,NULL);	
	//if((num_cameras+1)>2)
		//pthread_create(&video1_fd,NULL,video1_show,NULL);
	//if((num_cameras+1)>3)
		//pthread_create(&video2_fd,NULL,video2_show,NULL);
	//if((num_cameras+1)>4)
		//pthread_create(&video3_fd,NULL,video3_show,NULL);
    while(1){
			sleep(500);
		}
	pthread_detach(video0_fd);
    pthread_detach(video1_fd);
    pthread_detach(video2_fd);
    pthread_detach(video3_fd);
    pthread_join(video0_fd,NULL);
	pthread_join(video1_fd,NULL);
	pthread_join(video2_fd,NULL);
	pthread_join(video3_fd,NULL);

    return 0;
}