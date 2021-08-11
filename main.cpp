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
	short width = WINDOW_WIDTH, height = WINDOW_HEIGHT;		//初始化窗口、zbuffer深度缓存、framebuffer帧缓存
	float* zbuffer = new float[width * height];
	unsigned char* framebuffer = new unsigned char[width * height * 4];
	memset(framebuffer, 0, sizeof(unsigned char) * width * height * 4);

	Camera camera(Eye, Target, Up);

	mat4 model_mat;		
	model_mat.identity();												// 初始化ModelView矩阵
	mat4 view_mat = Model_View(camera.eye, camera.target, camera.up);	// ModelView矩阵
	mat4 perspective_mat = mat4_perspective(60, (float)(width) / height, -0.1, -10000);//透视矩阵

	srand((unsigned int)time(NULL));									//加载场景scene，着色器shader
	int model_num = 0;
	Model* model[MAX_MODEL_NUM];										//场景最大模型数，预设10
	IShader* shader_model;
	IShader* shader_skybox;

	std::string name;
	std::cout << "输入打开模型的名字：" << std::endl;					//可以改变输入方法
	//std::cin >> name;
	name = "yayi";
	build_scene(name, model, model_num, &shader_model, &shader_skybox, perspective_mat, &camera);

	window_init(width, height, "Sparrow");								//初始化窗口

	int num_frames = 0;													//统计总帧数
	float print_time = platform_get_time();								//获取平台时间
	bool is_buffer_update = true;										//是否更新帧缓存和深度缓存

	while (!window->is_close)											//循环一次是一帧
	{
		float curr_time = platform_get_time();

		if (is_buffer_update) {
			clear_framebuffer(width, height, framebuffer);					//清空帧缓存
			clear_zbuffer(width, height, zbuffer);							//清空zbuffer
		}
		
		handle_events(camera);											//更新相机坐标→键盘事件→鼠标事件→更新相机视角

		update_matrix(camera, view_mat, perspective_mat, shader_model, shader_skybox);//更新mvp矩阵

		if(is_buffer_update)
		for (int m = 0; m < model_num; m++)								//遍历场景Scene的模型 model_num
		{
			shader_model->payload.model = model[m];								//模型着色器
			if (shader_skybox != NULL) shader_skybox->payload.model = model[m];	//天空盒着色器

			IShader* shader;
			if (model[m]->is_skybox)									//当前模型是否是天空盒
				shader = shader_skybox;
			else
				shader = shader_model;

			for (int i = 0; i < model[m]->number_of_faces(); i++)		//遍历模型Model的三角形 model[m]->number_of_faces()
			{
				draw_triangles(framebuffer, zbuffer, *shader, i);
			}
		}

		num_frames += 1;												//帧率计算
		if (curr_time - print_time >= 1) {
			int sum_millis = (int)((curr_time - print_time) * 1000);
			int avg_millis = sum_millis / num_frames;
			printf(" fps: %3d, avg: %3d ms\n", num_frames, avg_millis);
			num_frames = 0;
			print_time = curr_time;
		}

		//如果没有输入信号则不更新framebuffer和zbuffer
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
	//内存释放
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