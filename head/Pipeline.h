#pragma once

#include "tools.h"
#include "Win.h"
#include "spinlock.hpp"
#include "../shader/shader.h"



//��դ��
void rasterize_singlethread(vec4* clipcoord_attri, unsigned char* framebuffer, float* zbuffer, IShader& shader);
//���߳� ����
//void rasterize_multithread(vec4* clipcoord_attri, unsigned char* framebuffer, float* zbuffer, IShader& shader);
//����������
void draw_triangles(unsigned char* framebuffer, float* zbuffer, IShader& shader, int nface);
