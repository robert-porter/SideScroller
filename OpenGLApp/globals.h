#pragma once

#include "Camera.h"
#include "Renderer.h"


struct Textures {
	TextureID gunAnim;
	TextureID anim;
	TextureID dude;
	TextureID sawblade;
};

struct Window {
	int width;
	int height;
	int id;
};

struct Mouse {
	int posX;
	int posY;
	bool leftButtonDown;
};



class World;

extern World		*world;
extern Renderer		renderer;
extern Camera		camera;
extern Mouse		mouse;
extern Window		window;
extern Textures		textures;

inline Vector2 ScreenToWorld(const Vector2 &screenPoint, const Camera &camera, const Window &window)
{
	Vector2 out = screenPoint;

	out.y = window.height - screenPoint.y;
	out -= Vector2(static_cast<float>(window.width)/2.0f, static_cast<float>(window.height)/2.0f);

	Vector2 cameraSize = camera.halfSize * 2.0f;

	out.x *= cameraSize.x/window.width;
	out.y *= cameraSize.y/window.height;

	out += camera.position;

	return out;
}
/*

*/
