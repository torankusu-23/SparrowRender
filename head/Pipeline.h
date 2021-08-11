#pragma once

#include "tools.h"
#include "Win.h"
#include "spinlock.hpp"
#include "../shader/shader.h"



//光栅化
void rasterize_singlethread(vec4* clipcoord_attri, unsigned char* framebuffer, float* zbuffer, IShader& shader);
//多线程 待定
//void rasterize_multithread(vec4* clipcoord_attri, unsigned char* framebuffer, float* zbuffer, IShader& shader);
//绘制三角形
void draw_triangles(unsigned char* framebuffer, float* zbuffer, IShader& shader, int nface);
