#pragma once
#include "struct.h"
#include "tools.h"
#include "windows.h"		//调用系统库

typedef struct mouse	//鼠标信息
{
	vec2 orbit_pos;		// 相机环绕位置
	vec2 orbit_delta;	// 环绕角度
	vec2 fv_pos;		// 第一人称位置
	vec2 fv_delta;
	float wheel_delta;	// 鼠标滚轮
}mouse_t;

typedef struct window	//窗口数据
{
	HWND h_window;		//窗口句柄
	HDC mem_dc;			//内存设备环境句柄
	HBITMAP bm_old;		//旧的位图句柄	
	HBITMAP bm_dib;		//设备无关的位图句柄
	unsigned char* window_fb;	//指向位图数据的指针
	int width;
	int height;
	char keys[512];		//键盘输入
	char buttons[2];	//鼠标输入，左键0， 右键1
	bool is_close;
	mouse_t mouse_info;
}window_t;

extern window_t* window;

int window_init(int width, int height, const char* title);	//窗口初始化
int window_destroy();			//窗口销毁
void window_draw(unsigned char* framebuffer);//窗口绘制
void msg_dispatch();			//消息分派
vec2 get_mouse_pos();			//鼠标位置
float platform_get_time(void);	//平台时间