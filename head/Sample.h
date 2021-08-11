#pragma once
#include "../shader/shader.h"


vec3 texture_sample(vec2 uv, Image* image);					//从纹理中采样纹素颜色
vec3 cubemap_sampling(vec3 direction, cubemap_t* cubemap);	//立方体图采样

void generate_prefilter_map(int thread_id, int face_id, int mip_level, Image& image);	//生成预过滤图，镜面反射部分？？？
void generate_irradiance_map(int thread_id, int face_id, Image& image);					//生成辐射图