#include "../head/Camera.h"
#include "../head/Win.h"
#include "../head/Controller.h"

Camera::Camera(vec3 e, vec3 t, vec3 up) : eye(e), target(t), up(up) {}

Camera::~Camera() {}

void updata_camera(Camera& camera)
{
	vec3 target_to_camera = camera.eye - camera.target;			// 目标点到相机的向量
	float radius = target_to_camera.norm();						// 相机到目标的距离，即旋转半径

	float phi = (float)atan2(target_to_camera[0], target_to_camera[2]); // azimuth angle(方位角), angle between target_to_camera and z-axis，[-pi, pi]
	float theta = (float)acos(target_to_camera[1] / radius);		  // zenith angle(天顶角), angle between target_to_camera and y-axis, [0, pi]
	float x_delta = window->mouse_info.orbit_delta[0] / window->width;
	float y_delta = window->mouse_info.orbit_delta[1] / window->height;

	radius *= (float)pow(0.95, window->mouse_info.wheel_delta);		//滚轮能通过调节radius，放缩相机与目标点的距离。距离目标越近，缩放幅度越小。

	float factor = 1.5 * PI;						//调节鼠标速度，鼠标左键控制旋转参数
	phi += x_delta * factor;						
	theta += y_delta * factor;
	if (theta > PI) theta = PI - EPSILON * 100;		//限制上下旋转角度0~180
	if (theta < 0)  theta = EPSILON * 100;

	camera.eye[0] = camera.target[0] + radius * sin(phi) * sin(theta);
	camera.eye[1] = camera.target[1] + radius * cos(theta);
	camera.eye[2] = camera.target[2] + radius * sin(theta) * cos(phi);

	
	factor = radius * (float)tan(60.0 / 360 * PI) * 2.2;// 鼠标右键控制位置参数
	x_delta = window->mouse_info.fv_delta[0] / window->width;
	y_delta = window->mouse_info.fv_delta[1] / window->height;
	vec3 left = x_delta * factor * camera.x;
	vec3 up = y_delta * factor * camera.y;

	camera.eye = camera.eye + (left - up);
	camera.target = camera.target + (left - up);
}

void handle_events(Camera& camera)
{
	camera.z = (camera.eye - camera.target).normalize();	//更新相机视角
	camera.x = cross(camera.up, camera.z).normalize();
	camera.y = cross(camera.z, camera.x).normalize();

	handle_key_events(camera);								//键盘事件
	handle_mouse_events(camera);							//鼠标事件
}



