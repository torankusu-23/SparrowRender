#include "../head/Controller.h"
#include "../head/Win.h"

//左键控制旋转角度，右键控制相机位置
void handle_mouse_events(Camera& camera)
{
	if (window->buttons[0])
	{
		vec2 cur_pos = get_mouse_pos();
		window->mouse_info.orbit_delta = window->mouse_info.orbit_pos - cur_pos;
		window->mouse_info.orbit_pos = cur_pos;
	}

	if (window->buttons[1])
	{
		vec2 cur_pos = get_mouse_pos();
		window->mouse_info.fv_delta = window->mouse_info.fv_pos - cur_pos;
		window->mouse_info.fv_pos = cur_pos;
	}

	updata_camera(camera);
}
//W,S,滚轮-控制缩放，Q\E\A\D,UP\DOWN\LEFT\RIGHT-控制相机位移
void handle_key_events(Camera& camera)
{
	float distance = (camera.target - camera.eye).norm();

	if (window->keys['W'])
	{
		camera.eye = camera.eye + (-10.0) / window->width * camera.z * distance;
	}
	if (window->keys['S'])
	{
		camera.eye = camera.eye + 0.05f * camera.z;
	}
	if (window->keys['Q'])
	{
		camera.eye = camera.eye + 0.05f * camera.y;
		camera.target = camera.target + 0.05f * camera.y;
	}
	if (window->keys['E'])
	{
		camera.eye = camera.eye + (-0.05f) * camera.y;
		camera.target = camera.target + (-0.05f) * camera.y;
	}
	if (window->keys['A'])
	{
		camera.eye = camera.eye + (-0.05f) * camera.x;
		camera.target = camera.target + (-0.05f) * camera.x;
	}
	if (window->keys['D'])
	{
		camera.eye = camera.eye + 0.05f * camera.x;
		camera.target = camera.target + 0.05f * camera.x;
	}
	if (window->keys[VK_ESCAPE])
	{
		window->is_close = 1;
	}
}