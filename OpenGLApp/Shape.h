#pragma once



#include "MathUtils.h"


enum ShapeType 
{
	eCircle = 0, 
	eCapsule = 1,
	eAABox = 2,
	eNumShapes = 3
};
class Renderer;

class Shape
{
public:

	Shape() { }

	virtual ~Shape() { }

	virtual Shape* Clone() const = 0;

	virtual ShapeType GetType() const = 0; 

	virtual float Area() const = 0;

	virtual void ComputeAABB(AABB& aabb, const Vector2 &worldPos) const = 0;

	virtual bool TestPoint(const Vector2 &point, const Vector2 &worldPos) const = 0;

	virtual void Draw(const Renderer &renderer, const Vector2 &worldPos) const = 0;
};

#include "Circle.h"
#include "Capsule.h"
#include "AABox.h"

class Circle;
class Capsule;
class AABox;

/*
	Distance
*/
float CirclesDistance(const Circle *circleA, const Vector2 &posA,
					  const Circle *circleB, const Vector2 &posB);

float CircleSegmentDistance(const Circle *circle, const Vector2 &circlePos,
							const Capsule *capsule, const Vector2 &capsulePos);

/*
	Overlap
*/

bool CheckCircleAABoxOverlap(const Circle *circle, const Vector2 &circlePos,
							 const AABox *box, const Vector2 &boxPos);



bool CheckCirclesOverlap(const Circle *circleA, const Vector2 &PosA,
						 const Circle *circleB, const Vector2 &posB);

bool CheckCircleCapsuleOverlap(const Circle *circle, const Vector2 &circlePos, 
							  const Capsule *capsule, const Vector2 &capsulePos);

bool CheckOverlap(const Shape *shapeA, const Vector2 &posA,
				  const Shape* shapeB, const Vector2 &posB);


/*
	Contact normal points from B to A
	contact point is the point on B
	the contact point on A is the contact point on B + depth * Normal
*/

bool FindOverlap(const Shape *shapeA, const Vector2 &posA,
				 const Shape* shapeB, const Vector2& posB,
				 Vector2 &contactNormal,
				 Vector2 &contactPoint,
				 float &contactDepth);

bool FindCircleAABoxOverlap(const Circle *circle, const Vector2 &circlePos,
				  			const AABox *box, const Vector2 &boxPos,
							Vector2 &contactNormal, 
							Vector2 &contactPoint, 
							float &contactDepth);

bool FindCircleCapsuleOverlap(const Circle *circle, const Vector2 &circlePos, 
							  const Capsule *capsule, const Vector2 &capsulePos, 
							  Vector2 &contactNormal,
							  Vector2 &contactPoint, 
							  float &contactDepth);

bool FindCirclesOverlap(const Circle *circleA, const Vector2 &posA,
				  		const Circle *circleB, const Vector2 &posB,
						Vector2 &contactNormal, 
						Vector2 &contactPoint, 
						float &contactDepth);

