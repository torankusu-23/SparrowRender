#include "../shader/shader.h"
#include "../head/Sample.h"

static vec3 cal_normal(vec3& normal, vec3* world_coords, const vec2* uvs, const vec2& uv, Image* normal_map);

void PhongShader::vertex_shader(int nfaces, int nvertex)
{	
	//获取面的v，vn，uv属性,加入点属性和裁剪点属性
	payload.tmp1_uv[nvertex] = payload.uv_attri[nvertex] = payload.model->uv_of_facei(nfaces, nvertex);
	payload.tmp1_clipcoord[nvertex] = payload.clipcoord_attri[nvertex] = payload.mvp_matrix * up_to_vec4(payload.model->v_of_facei(nfaces, nvertex), 1.0f);
	payload.tmp1_worldcoord[nvertex] = payload.worldcoord_attri[nvertex] = payload.model->v_of_facei(nfaces, nvertex);
	payload.tmp1_normal[nvertex] = payload.normal_attri[nvertex] = payload.model-> vn_of_facei(nfaces, nvertex);
}


vec3 PhongShader::fragment_shader(float alpha, float beta, float gamma)
{
	vec4* clip_coords = payload.clipcoord_attri;
	vec3* world_coords = payload.worldcoord_attri;
	vec3* normals = payload.normal_attri;
	vec2* uvs = payload.uv_attri;

	// interpolate attribute 透视校正
	float Z = 1.0 / (alpha / clip_coords[0].w() + beta / clip_coords[1].w() + gamma / clip_coords[2].w());
	vec3 normal = (alpha*normals[0] / clip_coords[0].w() + beta * normals[1] / clip_coords[1].w() +
		gamma * normals[2] / clip_coords[2].w()) * Z;
	vec2 uv = (alpha*uvs[0] / clip_coords[0].w() + beta * uvs[1] / clip_coords[1].w() +
		gamma * uvs[2] / clip_coords[2].w()) * Z;
	vec3 worldpos = (alpha*world_coords[0] / clip_coords[0].w() + beta * world_coords[1] / clip_coords[1].w() +
		gamma * world_coords[2] / clip_coords[2].w()) * Z;

	if (payload.model->normal_map)	//法线贴图修正
		normal = cal_normal(normal, world_coords, uvs, uv, payload.model->normal_map).normalize();

	//计算光照信息
	//环境光、漫反射光、镜面反射光参数
	vec3 ka(0.35, 0.35, 0.35);
	vec3 kd = payload.model->get_diffusemap(uv);
	vec3 ks(0.8, 0.8, 0.8);

	//设置默认的光照值
	float p = 150.0;
	vec3 l = vec3(1, 1, 1).normalize();
	vec3 light_ambient_intensity = kd;
	vec3 light_diffuse_intensity = vec3(0.9, 0.9, 0.9);
	vec3 light_specular_intensity = vec3(0.15, 0.15, 0.15);

	vec3 v = (payload.camera->eye - worldpos).normalize();
	vec3 h = (l + v).normalize();

	vec3 ambient = ka * light_ambient_intensity;	//简易环境光算法，没有计算环境光遮蔽，可以改进
	vec3 diffuse = (kd * light_diffuse_intensity) * fmax(0, dot(l, normal));	//纹理信息也埋在里面了，可以分离
	vec3 specular = (ks * light_specular_intensity) * fmax(0, pow(dot(normal, h), p));	//镜面反射算法，建议推一边，好像不对

	vec3 result_color = (ambient + diffuse + specular) * 255.f;
	return result_color;
}


void SkyboxShader::vertex_shader(int nfaces, int nvertex)	//和Phong模型的一样
{
	//获取面的v，vn，uv属性,加入点属性和输入点属性（用于裁剪）
	payload.tmp1_uv[nvertex] = payload.uv_attri[nvertex] = payload.model->uv_of_facei(nfaces, nvertex);
	payload.tmp1_clipcoord[nvertex] = payload.clipcoord_attri[nvertex] = payload.mvp_matrix * up_to_vec4(payload.model->v_of_facei(nfaces, nvertex), 1.0f);
	payload.tmp1_worldcoord[nvertex] = payload.worldcoord_attri[nvertex] = payload.model->v_of_facei(nfaces, nvertex);
	payload.tmp1_normal[nvertex] = payload.normal_attri[nvertex] = payload.model->vn_of_facei(nfaces, nvertex);
}


vec3 SkyboxShader::fragment_shader(float alpha, float beta, float gamma)
{
	vec4* clip_coords = payload.clipcoord_attri;
	vec3* world_coords = payload.worldcoord_attri;
	// 世界坐标透视校正
	float Z = 1.0 / (alpha / clip_coords[0].w() + beta / clip_coords[1].w() + gamma / clip_coords[2].w());
	vec3 worldpos = (alpha * world_coords[0] / clip_coords[0].w() + beta * world_coords[1] / clip_coords[1].w() + gamma * world_coords[2] / clip_coords[2].w()) * Z;

	vec3 result_color = cubemap_sampling(worldpos, payload.model->environment_map);//通过世界坐标，获取环境贴图颜色，貌似无关采样sampling，有待改名
	
	return result_color * 255.f;
}

//辅助工具函数---------------------------------------------------
static vec3 cal_normal(vec3& normal, vec3* world_coords, const vec2* uvs, const vec2& uv, Image* normal_map)
{
	// calculate the difference in UV coordinate
	float x1 = uvs[1][0] - uvs[0][0];
	float y1 = uvs[1][1] - uvs[0][1];
	float x2 = uvs[2][0] - uvs[0][0];
	float y2 = uvs[2][1] - uvs[0][1];
	float det = (x1 * y2 - x2 * y1);

	// calculate the difference in world pos
	vec3 e1 = world_coords[1] - world_coords[0];
	vec3 e2 = world_coords[2] - world_coords[0];

	vec3 t = e1 * y2 + e2 * (-y1);
	vec3 b = e1 * (-x2) + e2 * x1;
	t = t / det;
	b = b / det;

	// schmidt orthogonalization
	normal = normal.normalize();
	t = (t - dot(t, normal) * normal).normalize();
	b = (b - dot(b, normal) * normal - dot(b, t) * t).normalize();

	vec3 sample = texture_sample(uv, normal_map);
	// modify the range from 0 ~ 1 to -1 ~ +1
	sample = vec3(sample[0] * 2 - 1, sample[1] * 2 - 1, sample[2] * 2 - 1);

	vec3 normal_new = t * sample[0] + b * sample[1] + normal * sample[2];
	return normal_new;
}