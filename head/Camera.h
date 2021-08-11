#pragma once
#include "tools.h"
#include "struct.h"

class Camera
{
public:
	Camera(vec3 eye, vec3 target, vec3 up);
	~Camera();

	vec3 eye;
	vec3 target;
	vec3 up;
	vec3 x;
	vec3 y;
	vec3 z;
};

void updata_camera(Camera& camera);		//更新相机视角
void handle_events(Camera& camera);		//每帧：调用→更新相机坐标→调用键盘事件→调用鼠标事件→更新相机视角