#include "AABox.h"
#include "Renderer.h"

void AABox::Draw(const Renderer &renderer, const Vector2 &worldPos) const 
{
	AABB aabb;
	ComputeAABB(aabb, worldPos);
	renderer.DrawAABB(aabb, Color4f(0.4, 0.1, 0.9, 0.2f));
}