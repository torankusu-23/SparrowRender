#pragma once
#include "../head/Model.h"
#include "../head/Camera.h"

struct light
{
	vec3 pos;
	vec3 intensity;
};

typedef struct cubemap
{
	Image* faces[6];
}cubemap_t;
 
typedef struct iblmap
{
	int mip_levels;
	cubemap_t* irradiance_map;
	cubemap_t* prefilter_maps[15];
	Image* brdf_lut;
} iblmap_t;

typedef struct
{
	mat4 model_matrix;
	mat4 camera_view_matrix;
	mat4 light_view_matrix;
	mat4 camera_perp_matrix;
	mat4 light_perp_matrix;
	mat4 mvp_matrix;

	Camera* camera;
	Model* model;
	//������ͼ�Ĺ���
	iblmap_t* iblmap;

	// ��������
	vec3 normal_attri[3];
	vec2 uv_attri[3];
	vec3 worldcoord_attri[3];
	vec4 clipcoord_attri[3];

	// ���ڲü�������������ཫһ����������������ü���MAX_VERTEX������Ķ���Σ��������
	vec3 tmp1_normal[MAX_VERTEX];
	vec2 tmp1_uv[MAX_VERTEX];
	vec3 tmp1_worldcoord[MAX_VERTEX];
	vec4 tmp1_clipcoord[MAX_VERTEX];
	vec3 tmp2_normal[MAX_VERTEX];
	vec2 tmp2_uv[MAX_VERTEX];
	vec3 tmp2_worldcoord[MAX_VERTEX];
	vec4 tmp2_clipcoord[MAX_VERTEX];
}payload_t;

class IShader
{
public:
	payload_t payload;
	virtual void vertex_shader(int nfaces, int nvertex) {}
	virtual vec3 fragment_shader(float alpha, float beta, float gamma) { return vec3(0, 0, 0); }
};

class PhongShader :public IShader
{
public:
	void vertex_shader(int nfaces, int nvertex);
	vec3 fragment_shader(float alpha, float beta, float gamma);
};

class SkyboxShader :public IShader
{
public:
	void vertex_shader(int nfaces, int nvertex);
	vec3 fragment_shader(float alpha, float beta, float gamma);
};
/*
class PBRShader :public IShader
{
public:
	void vertex_shader(int nfaces, int nvertex);
	vec3 fragment_shader(float alpha, float beta, float gamma);
	vec3 direct_fragment_shader(float alpha, float beta, float gamma);
};
*/


