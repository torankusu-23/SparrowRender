#pragma once
#include "iostream"
#include "head/Model.h"
#include "head/Win.h"
#include "head/Camera.h"
#include "head/Controller.h"
#include "head/Pipeline.h"
#include "head/Scene.h"
#include <ctime>
#include <iostream>
#include "winuser.rh"
const vec3 Up(0, 1, 0);
const vec3 Eye(0, 1, 5);
const vec3 Target(0, 1, 0);

void update_matrix(Camera& camera, mat4 view_mat, mat4 perspective_mat, IShader* shader_model, IShader* shader_skybox);

int main()
{
	short width = WINDOW_WIDTH, height = WINDOW_HEIGHT;		//��ʼ�����ڡ�zbuffer��Ȼ��桢framebuffer֡����
	float* zbuffer = new float[width * height];
	unsigned char* framebuffer = new unsigned char[width * height * 4];
	memset(framebuffer, 0, sizeof(unsigned char) * width * height * 4);

	Camera camera(Eye, Target, Up);

	mat4 model_mat;		
	model_mat.identity();												// ��ʼ��ModelView����
	mat4 view_mat = Model_View(camera.eye, camera.target, camera.up);	// ModelView����
	mat4 perspective_mat = mat4_perspective(60, (float)(width) / height, -0.1, -10000);//͸�Ӿ���

	srand((unsigned int)time(NULL));									//���س���scene����ɫ��shader
	int model_num = 0;
	Model* model[MAX_MODEL_NUM];										//�������ģ������Ԥ��10
	IShader* shader_model;
	IShader* shader_skybox;

	std::string name;
	std::cout << "�����ģ�͵����֣�" << std::endl;					//���Ըı����뷽��
	//std::cin >> name;
	name = "yayi";
	build_scene(name, model, model_num, &shader_model, &shader_skybox, perspective_mat, &camera);

	window_init(width, height, "Sparrow");								//��ʼ������

	int num_frames = 0;													//ͳ����֡��
	float print_time = platform_get_time();								//��ȡƽ̨ʱ��
	bool is_buffer_update = true;										//�Ƿ����֡�������Ȼ���

	while (!window->is_close)											//ѭ��һ����һ֡
	{
		float curr_time = platform_get_time();

		if (is_buffer_update) {
			clear_framebuffer(width, height, framebuffer);					//���֡����
			clear_zbuffer(width, height, zbuffer);							//���zbuffer
		}
		
		handle_events(camera);											//�����������������¼�������¼�����������ӽ�

		update_matrix(camera, view_mat, perspective_mat, shader_model, shader_skybox);//����mvp����

		if(is_buffer_update)
		for (int m = 0; m < model_num; m++)								//��������Scene��ģ�� model_num
		{
			shader_model->payload.model = model[m];								//ģ����ɫ��
			if (shader_skybox != NULL) shader_skybox->payload.model = model[m];	//��պ���ɫ��

			IShader* shader;
			if (model[m]->is_skybox)									//��ǰģ���Ƿ�����պ�
				shader = shader_skybox;
			else
				shader = shader_model;

			for (int i = 0; i < model[m]->number_of_faces(); i++)		//����ģ��Model�������� model[m]->number_of_faces()
			{
				draw_triangles(framebuffer, zbuffer, *shader, i);
			}
		}

		num_frames += 1;												//֡�ʼ���
		if (curr_time - print_time >= 1) {
			int sum_millis = (int)((curr_time - print_time) * 1000);
			int avg_millis = sum_millis / num_frames;
			printf(" fps: %3d, avg: %3d ms\n", num_frames, avg_millis);
			num_frames = 0;
			print_time = curr_time;
		}

		//���û�������ź��򲻸���framebuffer��zbuffer
		if (window->mouse_info.wheel_delta == 0 && !window->buttons[0] && !window->buttons[1] && !window->keys['W'] && !window->keys['S'] && !window->keys['A'] && !window->keys['D'] && !window->keys['Q'] && !window->keys['E']){
			is_buffer_update = false;
		}
		else {
			is_buffer_update = true;
			window->mouse_info.wheel_delta = 0;
			window->mouse_info.orbit_delta = vec2(0, 0);
			window->mouse_info.fv_delta = vec2(0, 0);
		}

		window_draw(framebuffer);											//draw
		msg_dispatch();
	}
	//�ڴ��ͷ�
	for (int i = 0; i < model_num; i++)
		if (model[i] != NULL)  delete model[i];
	if (shader_model != NULL)  delete shader_model;
	if (shader_skybox != NULL) delete shader_skybox;
	delete [] zbuffer;
	delete [] framebuffer;
	window_destroy();

	system("pause");
	return 0;
}

void update_matrix(Camera& camera, mat4 view_mat, mat4 perspective_mat, IShader* shader_model, IShader* shader_skybox)
{
	view_mat = Model_View(camera.eye, camera.target, camera.up);	
	mat4 mvp = perspective_mat * view_mat;
	shader_model->payload.camera_view_matrix = view_mat;
	shader_model->payload.mvp_matrix = mvp;

	if (shader_skybox != NULL)
	{
		mat4 view_skybox = view_mat;
		view_skybox[0][3] = 0;
		view_skybox[1][3] = 0;
		view_skybox[2][3] = 0;
		shader_skybox->payload.camera_view_matrix = view_skybox;
		shader_skybox->payload.mvp_matrix = perspective_mat * view_skybox;
	}
}