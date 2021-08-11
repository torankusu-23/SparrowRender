#pragma once
#include "struct.h"
#include "tools.h"
#include "windows.h"		//����ϵͳ��

typedef struct mouse	//�����Ϣ
{
	vec2 orbit_pos;		// �������λ��
	vec2 orbit_delta;	// ���ƽǶ�
	vec2 fv_pos;		// ��һ�˳�λ��
	vec2 fv_delta;
	float wheel_delta;	// ������
}mouse_t;

typedef struct window	//��������
{
	HWND h_window;		//���ھ��
	HDC mem_dc;			//�ڴ��豸�������
	HBITMAP bm_old;		//�ɵ�λͼ���	
	HBITMAP bm_dib;		//�豸�޹ص�λͼ���
	unsigned char* window_fb;	//ָ��λͼ���ݵ�ָ��
	int width;
	int height;
	char keys[512];		//��������
	char buttons[2];	//������룬���0�� �Ҽ�1
	bool is_close;
	mouse_t mouse_info;
}window_t;

extern window_t* window;

int window_init(int width, int height, const char* title);	//���ڳ�ʼ��
int window_destroy();			//��������
void window_draw(unsigned char* framebuffer);//���ڻ���
void msg_dispatch();			//��Ϣ����
vec2 get_mouse_pos();			//���λ��
float platform_get_time(void);	//ƽ̨ʱ��