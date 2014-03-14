#include "Capsule.h"
#include "Renderer.h"


void Capsule::Draw(const Renderer &renderer, const Vector2 &worldPos) const 
{
	Vector2 pos0 = localPos0 + worldPos;
	Vector2 pos1 = localPos1 + worldPos;

	renderer.DrawCapsule(pos0, pos1, radius, Color4f(0.8f, 0.2f, 0.7f, 0.4f));

}