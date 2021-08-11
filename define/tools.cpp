#include "../head/tools.h"

mat4 mat4_translate(float tx, float ty, float tz)
{
	mat4 m;
	m.identity();
	m[0][3] = tx;
	m[1][3] = ty;
	m[2][3] = tz;
	return m;
}

mat4 mat4_scale(float sx, float sy, float sz)
{
	mat4 m;
	m.identity();
	m[0][0] = sx;
	m[1][1] = sy;
	m[2][2] = sz;
	return m;
}

/*
 * angle: the angle of rotation, in degrees
 *
 *  1  0  0  0
 *  0  c -s  0
 *  0  s  c  0
 *  0  0  0  1
 *
 */
mat4 mat4_rotate_x(float angle)
{
	mat4 m;
	m.identity();
	angle = angle / 180.0 * PI;
	float c = cos(angle);
	float s = sin(angle);
	m[1][1] = c;
	m[1][2] = -s;
	m[2][1] = s;
	m[2][2] = c;
	return m;

}

/*
 * angle: the angle of rotation, in degrees
 *
 *  c  0  s  0
 *  0  1  0  0
 * -s  0  c  0
 *  0  0  0  1
 *
 */
mat4 mat4_rotate_y(float angle)
{
	mat4 m;
	m.identity();
	angle = angle / 180.0 * PI;
	float c = cos(angle);
	float s = sin(angle);
	m[0][0] = c;
	m[0][2] = s;
	m[2][0] = -s;
	m[2][2] = c;
	return m;
}

/*
 * angle: the angle of rotation, in degrees
 *
 *  c -s  0  0
 *  s  c  0  0
 *  0  0  1  0
 *  0  0  0  1
 *
 */
mat4 mat4_rotate_z(float angle)
{
	mat4 m;
	m.identity();
	angle = angle / 180.0 * PI;
	float c = cos(angle);
	float s = sin(angle);
	m[0][0] = c;
	m[0][1] = -s;
	m[1][0] = s;
	m[1][1] = c;
	return m;
}


/*
 * x_axis.x  x_axis.y  x_axis.z  -dot(x_axis,eye)
 * y_axis.x  y_axis.y  y_axis.z  -dot(y_axis,eye)
 * z_axis.x  z_axis.y  z_axis.z  -dot(z_axis,eye)
 *        0         0         0                 1
 */
mat4 Model_View(vec3 eye, vec3 target, vec3 up)
{/*
	vec3 z =(eye - target).normalize();
	vec3 x = cross(up, z).normalize();
	vec3 y = cross(z, x).normalize();
	mat4 view;
	view.identity();

	for (int i = 0; i < 3; ++i) {	//0，1，2行存储x,y,z轴坐标，第四行固定0001，第四列存储一个位移
		view[0][i] = x[i];			//移动相机的原理是移动整个世界场景坐标，相机坐标移动分为两步，第一步是位移Mt，第二步是选择Mr
		view[1][i] = y[i];
		view[2][i] = z[i];
	}

	view[0][3] = -dot(x, eye);		//Mt * Mr得到的第四列结果。参考 http://www.songho.ca/opengl/gl_camera.html
	view[1][3] = -dot(y, eye);
	view[2][3] = -dot(z, eye);

	return view;*/
	mat4 m;
	m.identity();

	vec3 z = (eye - target).normalize();
	vec3 x = cross(up, z).normalize();
	vec3 y = cross(z, x).normalize();
	//std::cout << eye - target << std::endl;
	//std::cout << cross(up, z) << std::endl;
	//std::cout << cross(z, x) << std::endl;

	m[0][0] = x[0];
	m[0][1] = x[1];
	m[0][2] = x[2];

	m[1][0] = y[0];
	m[1][1] = y[1];
	m[1][2] = y[2];

	m[2][0] = z[0];
	m[2][1] = z[1];
	m[2][2] = z[2];

	m[0][3] = -dot(x, eye); //相当于原来要位移的，在新的坐标系下是位移多少，有个改变
	m[1][3] = -dot(y, eye);
	m[2][3] = -dot(z, eye);

	return m;
}


/*
*	2 / (r - l)		0				0			- (r + l) / (r - l)
*		0			2 / (t - b)     0			- (t + b) / (t - b)
*		0			0				2/(n - f)	- (f + n) / (n - f)
*		0			0				0					1
*/
mat4 mat4_ortho(float left, float right, float bottom, float top, float near, float far)
{
	float x_range = right - left;
	float y_range = top - bottom;
	float z_range = near - far;
	mat4 m;
	m.identity();
	m[0][0] = 2 / x_range;
	m[1][1] = 2 / y_range;
	m[2][2] = 2 / z_range;
	m[0][3] = -(left + right) / x_range;
	m[1][3] = -(bottom + top) / y_range;
	m[2][3] = -(near + far) / z_range;
	return m;
}


/*
 * 1/(aspect*tan(fovy/2))              0             0           0
 *                      0  1/tan(fovy/2)             0           0
 *                      0              0  -(f+n)/(f-n)  -2fn/(f-n)
 *                      0              0            -1           0
 */
mat4 mat4_perspective(float fovy, float aspect, float near, float far)
{
	mat4 view;
	view.identity();
	fovy = fovy / 180.0 * PI;			
	float t = fabs(near) * tan(fovy / 2);	//求top
	float r = aspect * t;					//求right，aspect表示宽高比

	view[0][0] = near / r;					//推导  http://www.songho.ca/opengl/gl_projectionmatrix.html
	view[1][1] = near / t;
	view[2][2] = (near + far) / (near - far);
	view[2][3] = 2 * near * far / (far - near);
	view[3][2] = 1;
	view[3][3] = 0;
	return view;
}

float float_clamp(float f, float min, float max)
{
	return f < min ? min : (f > max ? max : f);
}

float float_lerp(float start, float end, float alpha)
{
	return start + (end - start) * alpha;
}

vec2 vec2_lerp(vec2& start, vec2& end, float alpha)
{
	return start + (end - start) * alpha;
}

vec3 vec3_lerp(vec3& start, vec3& end, float alpha)
{
	return start + (end - start) * alpha;
}

vec4 vec4_lerp(vec4& start, vec4& end, float alpha)
{
	return start + (end - start) * alpha;
}


//-----------------------------------------------------------------

void clear_zbuffer(int width, int height, float* zbuffer)
{
	for (int i = 0; i < width * height; i++)
		zbuffer[i] = 100000;
}

void clear_framebuffer(int width, int height, unsigned char* framebuffer)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int index = (i * width + j) * 4;

			framebuffer[index + 2] = 80;
			framebuffer[index + 1] = 56;
			framebuffer[index] = 56;
		}
	}
}
