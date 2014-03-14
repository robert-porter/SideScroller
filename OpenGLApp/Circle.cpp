#include "Circle.h"
#include "Renderer.h"

void Circle::Draw(const Renderer &renderer, const Vector2 &worldPos) const {
	Vector2 pos = localPos + worldPos;
	renderer.DrawCircle(pos, radius, Color4f(1.0f, 0.0f, 0.0f, 8.0f));
}