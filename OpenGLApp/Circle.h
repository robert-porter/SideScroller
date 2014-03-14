#pragma once


#include "MathUtils.h"
#include "Shape.h"


class Renderer;

class Circle : public Shape { 
public:

	Circle();

	Shape* Clone() const;

	ShapeType GetType() const;

	float Area() const;

	bool TestPoint(const Vector2 &point, const Vector2 &worldPos) const;
	
	void ComputeAABB(AABB& aabb, const Vector2 &worldPos) const;

	void Draw(const Renderer &renderer, const Vector2 &worldPos) const;

	Vector2 localPos;

	float radius;
};


/*

*/

inline Circle::Circle() : localPos(0.0f, 0.0f) {

}	

inline Shape* Circle::Clone() const {
	Circle *clone = new Circle;
	clone->localPos = localPos;
	clone->radius = radius;
	return clone;
}

inline ShapeType Circle::GetType( ) const {
	return eCircle; 
} 

inline float Circle::Area( ) const {
	return PI * radius * radius;
}

inline bool Circle::TestPoint(const Vector2 &point, const Vector2 &worldPos) const {
	Vector2 cirPos = localPos + worldPos;
	float radius2 = radius * radius;
	float dis2 = length2(cirPos - point);
	
	return radius2 > dis2;
}

inline void Circle::ComputeAABB(AABB& aabb, const Vector2 &worldPos) const {
	Vector2 cirPos = localPos + worldPos;
	Vector2 R = Vector2(radius, radius);

	aabb.minExt = cirPos - R;
	aabb.maxExt = cirPos + R;
}

