#include "../head/Pipeline.h"
static SpinLock sp;
static bool is_back_facing(vec3 ndc_pos[3]);
static vec3 compute_barycentric2D(float x, float y, const vec3* v);
static void set_color(unsigned char* framebuffer, int x, int y, unsigned char color[]);
static bool is_inside_triangle(float alpha, float beta, float gamma);
static int get_index(int x, int y);

typedef enum
{
	W_PLANE,
	X_RIGHT,
	X_LEFT,
	Y_TOP,
	Y_BOTTOM,
	Z_NEAR,
	Z_FAR
} clip_plane;
static int is_inside_plane(clip_plane c_plane, vec4 vertex);
static float get_intersect_ratio(vec4 prev, vec4 curv, clip_plane c_plane);
static int clip_with_plane(clip_plane c_plane, int num_vert, payload_t& payload);
static int homo_clipping(payload_t& payload);
static void transform_attri(payload_t& payload, int index0, int index1, int index2);


void rasterize_singlethread(vec4* clipcoord_attri, unsigned char* framebuffer, float* zbuffer, IShader& shader) {
	vec3 ndc_pos[3];
	vec3 screen_pos[3];
	unsigned char c[3];
	int width = window->width;
	int height = window->height;
	bool is_skybox = shader.payload.model->is_skybox;

	// homogeneous division ͸�ӳ���
	for (int i = 0; i < 3; i++)
	{
		ndc_pos[i][0] = clipcoord_attri[i][0] / clipcoord_attri[i].w();
		ndc_pos[i][1] = clipcoord_attri[i][1] / clipcoord_attri[i].w();
		ndc_pos[i][2] = clipcoord_attri[i][2] / clipcoord_attri[i].w();
	}

	// viewport transformation �ӿڱ任
	for (int i = 0; i < 3; i++)
	{
		screen_pos[i][0] = 0.5 * (width - 1) * (ndc_pos[i][0] + 1.0);
		screen_pos[i][1] = 0.5 * (height - 1) * (ndc_pos[i][1] + 1.0);
		screen_pos[i][2] = is_skybox ? 1000 : -clipcoord_attri[i].w();	//view space z-value ��պй̶�z��1000��һ�������βü�����w,why��
	}

	// backface clip (skybox didnit need it) �޳�����������
	if (!is_skybox)
	{
		if (is_back_facing(ndc_pos)) return;
	}

	// get bounding box ��Χ��
	float xmin = 10000, xmax = -10000, ymin = 10000, ymax = -10000;
	for (int i = 0; i < 3; i++)
	{
		xmin = fmin(xmin, screen_pos[i][0]);
		xmax = fmax(xmax, screen_pos[i][0]);
		ymin = fmin(ymin, screen_pos[i][1]);
		ymax = fmax(ymax, screen_pos[i][1]);
	}

	// rasterization ��դ��
	for (int x = (int)xmin; x <= (int)xmax; ++x)
	{
		for (int y = (int)ymin; y <= (int)ymax; ++y)
		{
			vec3 barycentric = compute_barycentric2D((float)(x + 0.5), (float)(y + 0.5), screen_pos);
			float alpha = barycentric.x(); float beta = barycentric.y(); float gamma = barycentric.z();

			if (is_inside_triangle(alpha, beta, gamma))
			{
				int index = get_index(x, y);	//��ȡ������zbuffer����
				// z����͸��У��
				float normalizer = 1.0 / (alpha / clipcoord_attri[0].w() + beta / clipcoord_attri[1].w() + gamma / clipcoord_attri[2].w());
				//for larger z means away from camera, needs to interpolate z-value as a property			
				float z = (alpha * screen_pos[0].z() / clipcoord_attri[0].w() + beta * screen_pos[1].z() / clipcoord_attri[1].w() +
					gamma * screen_pos[2].z() / clipcoord_attri[2].w()) * normalizer;

				if (zbuffer[index] > z)
				{
					zbuffer[index] = z;
					vec3 color = shader.fragment_shader(alpha, beta, gamma);

					for (int i = 0; i < 3; i++)
					{
						c[i] = (int)float_clamp(color[i], 0, 255);
					}
					set_color(framebuffer, x, y, c);
				}
			}
		}
	}
}


void draw_triangles(unsigned char* framebuffer, float* zbuffer, IShader& shader, int nface)
{
	for (int i = 0; i < 3; i++)	//����ɫ
	{
		shader.vertex_shader(nface, i);
	}

	int num_vertex = homo_clipping(shader.payload);// homogeneous clipping ��βü�

	for (int i = 0; i < num_vertex - 2; i++) {// triangle assembly and reaterize ��װ������
		int index0 = 0;
		int index1 = i + 1;
		int index2 = i + 2;

		transform_attri(shader.payload, index0, index1, index2);// ��������װ��
		rasterize_singlethread(shader.payload.clipcoord_attri, framebuffer, zbuffer, shader);// ��դ��������
	}
}

//���ߺ���----------------------------------------------------------------
static bool is_back_facing(vec3 ndc_pos[3])
{
	vec3 a = ndc_pos[0];
	vec3 b = ndc_pos[1];
	vec3 c = ndc_pos[2];
	float signed_area = a.x() * b.y() - a.y() * b.x() +
		b.x() * c.y() - b.y() * c.x() +
		c.x() * a.y() - c.y() * a.x(); 
	return signed_area <= 0;
}

static vec3 compute_barycentric2D(float x, float y, const vec3* v)
{
	float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
	float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
	return vec3(c1, c2, 1 - c1 - c2);
}

static void set_color(unsigned char* framebuffer, int x, int y, unsigned char color[])
{
	int index = ((WINDOW_HEIGHT - y - 1) * WINDOW_WIDTH + x) * 4; // the origin for pixel is bottom-left, but the framebuffer index counts from top-left
	for (int i = 0; i < 3; i++)
		framebuffer[index + i] = color[i];
}

static bool is_inside_triangle(float alpha, float beta, float gamma)
{
	if (alpha > -EPSILON && beta > -EPSILON && gamma > -EPSILON) return true;
	return false;
}

static int get_index(int x, int y)
{
	return (WINDOW_HEIGHT - y - 1) * WINDOW_WIDTH + x;
}

static int is_inside_plane(clip_plane c_plane, vec4 vertex)
{
	switch (c_plane)
	{
	case W_PLANE:
		return vertex.w() <= -EPSILON;		//����w>0�ĵ㣬��ֹ�ü�֮���͸�ӳ����׶γ��ֳ���0�Ĵ���
	case X_RIGHT:							//���ģ�������е�wֵ���Ǹ�ֵ
		return vertex.x() >= vertex.w();
	case X_LEFT:
		return vertex.x() <= -vertex.w();
	case Y_TOP:
		return vertex.y() >= vertex.w();
	case Y_BOTTOM:
		return vertex.y() <= -vertex.w();
	case Z_NEAR:
		return vertex.z() >= vertex.w();
	case Z_FAR:
		return vertex.z() <= -vertex.w();
	default:
		return 0;
	}
}

static float get_intersect_ratio(vec4 prev, vec4 curv, clip_plane c_plane)
{
	switch (c_plane)
	{
	case W_PLANE:
		return (prev.w() + EPSILON) / (prev.w() - curv.w());
	case X_RIGHT:
		return (prev.w() - prev.x()) / ((prev.w() - prev.x()) - (curv.w() - curv.x()));
	case X_LEFT:
		return (prev.w() + prev.x()) / ((prev.w() + prev.x()) - (curv.w() + curv.x()));
	case Y_TOP:
		return (prev.w() - prev.y()) / ((prev.w() - prev.y()) - (curv.w() - curv.y()));
	case Y_BOTTOM:
		return (prev.w() + prev.y()) / ((prev.w() + prev.y()) - (curv.w() + curv.y()));
	case Z_NEAR:
		return (prev.w() - prev.z()) / ((prev.w() - prev.z()) - (curv.w() - curv.z()));
	case Z_FAR:
		return (prev.w() + prev.z()) / ((prev.w() + prev.z()) - (curv.w() + curv.z()));
	default:
		return 0;
	}
}

static int clip_with_plane(clip_plane c_plane, int num_vert, payload_t& payload)
{
	int out_vert_num = 0;
	int previous_index, current_index;
	int is_odd = (c_plane + 1) % 2; //1 0 1 0 1 0 1

	// �ü�ƽ�����롢��������б�ָ�룬���������tmp1��tmp2֮�佻���洢��
	vec4* input_clipcoord = is_odd ? payload.tmp1_clipcoord : payload.tmp2_clipcoord;
	vec3* input_worldcoord = is_odd ? payload.tmp1_worldcoord : payload.tmp2_worldcoord;
	vec3* input_normal = is_odd ? payload.tmp1_normal : payload.tmp2_normal;
	vec2* input_uv = is_odd ? payload.tmp1_uv : payload.tmp2_uv;
	vec4* output_clipcoord = is_odd ? payload.tmp2_clipcoord : payload.tmp1_clipcoord;
	vec3* output_worldcoord = is_odd ? payload.tmp2_worldcoord : payload.tmp1_worldcoord;
	vec3* output_normal = is_odd ? payload.tmp2_normal : payload.tmp1_normal;
	vec2* output_uv = is_odd ? payload.tmp2_uv : payload.tmp1_uv;

	// tranverse all the edges from first vertex
	for (int i = 0; i < num_vert; i++)
	{
		current_index = i;
		previous_index = (i - 1 + num_vert) % num_vert;	//��һ����
		vec4 cur_vertex = input_clipcoord[current_index];//����ֹ��
		vec4 pre_vertex = input_clipcoord[previous_index];//����ʼ��

		int is_cur_inside = is_inside_plane(c_plane, cur_vertex);
		int is_pre_inside = is_inside_plane(c_plane, pre_vertex);

		if (is_cur_inside != is_pre_inside)	//������ֲ���ƽ�����࣬����ƽ�潻�������ڲ���
		{
			float ratio = get_intersect_ratio(pre_vertex, cur_vertex, c_plane);	//��ȡ��ƽ�潻���������������������֮����߶Σ�

			output_clipcoord[out_vert_num] = vec4_lerp(pre_vertex, cur_vertex, ratio);//��ֵ����ƽ���Ϸָ��Ĳ������÷ָ�����������
			output_worldcoord[out_vert_num] = vec3_lerp(input_worldcoord[previous_index], input_worldcoord[current_index], ratio);
			output_normal[out_vert_num] = vec3_lerp(input_normal[previous_index], input_normal[current_index], ratio);
			output_uv[out_vert_num] = vec2_lerp(input_uv[previous_index], input_uv[current_index], ratio);

			out_vert_num++;
		}

		if (is_cur_inside)					//��ֹ�����ڲ࣬����յ������ڲ���
		{
			output_clipcoord[out_vert_num] = cur_vertex;
			output_worldcoord[out_vert_num] = input_worldcoord[current_index];
			output_normal[out_vert_num] = input_normal[current_index];
			output_uv[out_vert_num] = input_uv[current_index];

			out_vert_num++;
		}
	}

	return out_vert_num;
}

static int homo_clipping(payload_t& payload)
{
	int num_vertex = 3;
	num_vertex = clip_with_plane(W_PLANE, num_vertex, payload); 	
	num_vertex = clip_with_plane(X_RIGHT, num_vertex, payload);
	num_vertex = clip_with_plane(X_LEFT, num_vertex, payload);	
	num_vertex = clip_with_plane(Y_TOP, num_vertex, payload);	
	num_vertex = clip_with_plane(Y_BOTTOM, num_vertex, payload);
	num_vertex = clip_with_plane(Z_NEAR, num_vertex, payload);	
	num_vertex = clip_with_plane(Z_FAR, num_vertex, payload);	
	return num_vertex;
}

static void transform_attri(payload_t& payload, int index0, int index1, int index2)
{
	payload.clipcoord_attri[0] = payload.tmp2_clipcoord[index0];
	payload.clipcoord_attri[1] = payload.tmp2_clipcoord[index1];
	payload.clipcoord_attri[2] = payload.tmp2_clipcoord[index2];
	payload.worldcoord_attri[0] = payload.tmp2_worldcoord[index0];
	payload.worldcoord_attri[1] = payload.tmp2_worldcoord[index1];
	payload.worldcoord_attri[2] = payload.tmp2_worldcoord[index2];
	payload.normal_attri[0] = payload.tmp2_normal[index0];
	payload.normal_attri[1] = payload.tmp2_normal[index1];
	payload.normal_attri[2] = payload.tmp2_normal[index2];
	payload.uv_attri[0] = payload.tmp2_uv[index0];
	payload.uv_attri[1] = payload.tmp2_uv[index1];
	payload.uv_attri[2] = payload.tmp2_uv[index2];
}

