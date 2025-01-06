#ifndef __EXAMPLE_H
#define __EXAMPLE_H


void errno_exit(const char *s);
void open_device(int video_id);
int get_camera_device_fmt(int video_id);
int init_device(int video_id);
void start_capturing(int video_id);
void mainloop(int video_id);
void mainloop_1(int video_id);
void mainloop_2(int video_id);
void mainloop_3(int video_id);
void *video0_show(void *arg);
void *video1_show(void *arg);
void *video2_show(void *arg);
void *video3_show(void *arg);

#endif