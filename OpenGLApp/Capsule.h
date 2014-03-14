#pragma once
#include "Shape.h"


class Capsule : public Shape {
public:
	Capsule();

	Shape* Clone( ) const;

	ShapeType GetType() const;

	float Area() const;
	
	bool TestPoint(const Vector2 &point, const Vector2 &worldPos) const;
	
	void ComputeAABB(AABB& aabb, const Vector2 &worldPos) const;

	void Draw(const Renderer &renderer, const Vector2 &worldPos) const;

	Vector2 localPos0;
	Vector2 localPos1;
	float radius;
};



inline Capsule::Capsule() { 

}
	
inline Shape* Capsule::Clone( ) const {
	Capsule *clone = new Capsule;
	clone->localPos0 = localPos0;
	clone->localPos1 = localPos1;
	clone->radius = radius;
	return clone;
}

inline ShapeType Capsule::GetType() const {
	return eCapsule;
} 

inline float Capsule::Area( ) const {
	return PI * radius * radius + length( localPos1 - localPos0 ) * radius;
}
	
inline bool Capsule::TestPoint(const Vector2 &point, const Vector2 &worldPos) const {
	Vector2 segPos0 = localPos0 + worldPos;
	Vector2 segPos1 = localPos1 + worldPos;
	float radius2 = radius * radius; 

	return DistancePointSegmentSq(point, segPos0, segPos1) < radius2;
}


inline void Capsule::ComputeAABB(AABB& aabb, const Vector2 &worldPos) const {
	Vector2 segPos0 = localPos0 + worldPos;
	Vector2 segPos1 = localPos1 + worldPos;

	Vector2 R = Vector2(radius, radius);

	float xMin = Min(segPos0.x, segPos1.x);
	float xMax = Max(segPos0.x, segPos1.x);
	float yMin = Min(segPos0.y, segPos1.y);
	float yMax = Min(segPos0.y, segPos1.y);

	Vector2 vMin(xMin, yMin);
	Vector2 vMax(xMax, yMax);

	aabb.minExt = vMin - R;
	aabb.maxExt = vMax + R;
}
