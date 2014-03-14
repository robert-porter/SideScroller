#pragma once

#include "Shape.h"


class AABox : public Shape, public AABB {
public:

	Shape*		Clone() const;

	ShapeType	GetType( ) const;

	float		Area() const;

	void		ComputeAABB(AABB& aabb, const Vector2 &worldPos) const;

	bool		TestPoint(const Vector2 &point, const Vector2 &worldPos) const;

	void		Draw(const Renderer &renderer, const Vector2 &worldPos) const;
};


inline Shape* AABox::Clone() const {
	AABox *clone = new AABox;
	clone->minExt = minExt;
	clone->maxExt = maxExt;
	return clone;
}

inline ShapeType AABox::GetType( ) const { 
	return eAABox; 
} 

inline float AABox::Area() const {
	Vector2 size = maxExt - minExt;
	return size.x * size.y;
}

inline void AABox::ComputeAABB(AABB& aabb, const Vector2 &worldPos) const {
	aabb.minExt = minExt + worldPos;
	aabb.maxExt = maxExt + worldPos;
}

inline bool AABox::TestPoint(const Vector2 &point, const Vector2 &worldPos) const {
	Vector2 worldMinExt = minExt + worldPos;
	Vector2 worldMaxExt = maxExt + worldPos;

	return  (point.x > worldMinExt.x &&
			 point.x < worldMaxExt.x &&
			 point.y > worldMinExt.y &&
			 point.y < worldMaxExt.y);
}