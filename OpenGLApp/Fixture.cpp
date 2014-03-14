#include "Fixture.h"
#include "Body.h"


inline bool Fixture::TestPoint(const Vector2& point) const
{
	return shape->TestPoint(point, body->position);
}

inline void Fixture::GetAABB(AABB& aabb) const
{
	shape->ComputeAABB(aabb, body->position);
}