#pragma once

#include "MathUtils.h"



class Camera
{
public:
	Camera(void) : position(0.0f,0.0f), halfSize(4.0f, 4.0f) { }

	Vector2 position;
	Vector2 halfSize;
};
