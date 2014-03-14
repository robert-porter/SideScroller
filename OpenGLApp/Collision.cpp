#include "Shape.h"

const float epsilon = 0.000001f;


float CirclesDistance(const Circle *circleA, const Vector2 &posA,
					  const Circle *circleB, const Vector2 &posB)
{
	Vector2 pA = circleA->localPos + posA;
	Vector2 pB = circleB->localPos + posB;
	float radius = circleA->radius + circleB->radius; 
	Vector2 AB = pB - pA;

	return glm::length( AB ) - radius;
}

float CircleSegmentDistance(const Circle *circle, const Vector2 &circlePos,
							const Capsule *capsule, const Vector2 &capsulePos)
{
	Vector2 segPos0 = capsule->localPos0 + capsulePos;
	Vector2 segPos1 = capsule->localPos1 + capsulePos;
	Vector2 cirPos = circle->localPos + circlePos;
	float radius = capsule->radius + circle->radius; 

	float dis2 = DistancePointSegmentSq(cirPos, segPos0, segPos1);

	return sqrtf(dis2);
}

bool CheckCircleAABoxOverlap(const Circle *circle, const Vector2 &circlePos, 
							 const AABox *box, const Vector2 &boxPos)
{
	Vector2 cirPos = circle->localPos + circlePos; 
	Vector2 bMin = box->minExt + boxPos;
	Vector2 bMax = box->maxExt + boxPos;
	float radius2 = circle->radius * circle->radius;

	float closestX;
	float closestY;

	if ( cirPos.x < bMin.x ) closestX = bMin.x;
	else if ( cirPos.x > bMax.x ) closestX = bMax.x;
	else closestX = cirPos.x;

	if ( cirPos.y < bMin.y ) closestY = bMin.y;
	else if ( cirPos.y > bMax.y ) closestY = bMax.y;
	else closestY = cirPos.y;

	Vector2 closestPoint(closestX, closestY);

	return length2( closestPoint - cirPos ) < radius2;
}


bool CheckCircleCapsuleOverlap(const Circle *circle, const Vector2 &circlePos,
							   const Capsule *capsule, const Vector2 &capsulePos)
{
	Vector2 segPos0 = capsule->localPos0 + capsulePos;
	Vector2 segPos1 = capsule->localPos1 + capsulePos;
	Vector2 cirPos = circle->localPos + circlePos;
	float radius = capsule->radius + circle->radius; 
	float radius2 = radius * radius; 

	return DistancePointSegmentSq(cirPos, segPos0, segPos1) < radius2;
}


bool CheckCirclesOverlap(const Circle *circleA, const Vector2 &posA,
				 		 const Circle *circleB, const Vector2 &posB)
{
	Vector2 A = circleA->localPos + posA;
	Vector2 B = circleB->localPos + posB;
	float radius = circleA->radius + circleB->radius; 
	float radius2 = radius*radius; 

	Vector2 AB = B - A;

	return length2(AB) < radius2;
}

bool CheckOverlap(const Shape *shapeA, const Vector2 &posA,
				  const Shape* shapeB, const Vector2& posB)
{
	if(shapeA->GetType() == eCircle && shapeB->GetType() == eCapsule)
		return CheckCircleCapsuleOverlap((Circle *)shapeA, posA, (Capsule *)shapeB, posB);

	else if(shapeA->GetType() == eCapsule && shapeB->GetType() == eCircle)
		return CheckCircleCapsuleOverlap((Circle *)shapeB, posB, (Capsule *)shapeA, posA);

	else if(shapeA->GetType() == eCircle && shapeB->GetType() == eCircle)
		return CheckCirclesOverlap((Circle *)shapeA, posA, (Circle *)shapeB, posB);

	else
	{
		assert(!"Bad shape types");
		return false;
	}
}

bool FindCircleAABoxOverlap(const Circle *circle, const Vector2 &circlePos,
				  			const AABox *box, const Vector2 &boxPos,
							Vector2 &contactNormal, 
							Vector2 &contactPoint, 
							float &contactDepth)
{
	Vector2 cirPos = circle->localPos + circlePos; 
	Vector2 bMin = box->minExt + boxPos;
	Vector2 bMax = box->maxExt + boxPos;
	float radius = circle->radius;
	float radius2 = circle->radius * circle->radius;

	float closestX;
	float closestY;

	if ( cirPos.x < bMin.x ) closestX = bMin.x;
	else if ( cirPos.x > bMax.x ) closestX = bMax.x;
	else closestX = cirPos.x;

	if ( cirPos.y < bMin.y ) closestY = bMin.y;
	else if ( cirPos.y > bMax.y ) closestY = bMax.y;
	else closestY = cirPos.y;

	contactPoint = Vector2(closestX, closestY);

	contactNormal = cirPos - contactPoint;

	if (  length2( contactNormal ) > radius2 )
		return false;

	float lenN = length(contactNormal);

	if (  lenN < 0.0001f )
	{
		contactNormal=Vector2(0.0f, 1.0f);
	}
	else
	{
		contactNormal/=lenN;
	}
	contactDepth = radius - lenN;
	return true;
}


bool FindCircleCapsuleOverlap(const Circle *circle, const Vector2 &circlePos, 
							 const Capsule *capsule, const Vector2 &capsulePos, 
							 Vector2 &contactNormal,
							 Vector2 &contactPoint, 
							 float &contactDepth)
{
	Vector2 segA = capsule->localPos0 + capsulePos;
	Vector2 segB = capsule->localPos1 + capsulePos;
	Vector2 cirP = circle->localPos + circlePos;
	float radius = capsule->radius + circle->radius;
	float radius2 = radius * radius;

	Vector2 AB = segB - segA;
	Vector2 AP = cirP - segA;


	if(dot(AB, AP) < 0) // closest to A
	{	
		float ap2 = length2( AP );

		if(ap2 < radius2 )
		{
			float ap = sqrtf(ap2);
			contactDepth = radius-ap;
			if ( ap > epsilon )
				contactNormal = AP / ap;
			else
				contactNormal = Vector2(0.0f, 1.0f);
			contactPoint = segA + contactNormal * capsule->radius;
			return true;
		}
		return false;
	}
	
	Vector2 BP = cirP - segB;
	Vector2 BA = -AB;
	
	if(dot(BP, BA) < 0.0f ) // closest to B
	{
		float bp2 = length2( BP );

		if( bp2 < radius2)
		{
			float bp = sqrtf(bp2);
			contactDepth = radius-bp;
			if ( bp > epsilon )
				contactNormal = BP/ bp;
			else
				contactNormal = Vector2(0.0f, 1.0f);
			contactPoint = segB + contactNormal * capsule->radius;
			return true;
		}
		return false;
	}

	float projAPonAB = dot(AB, AP) / glm::dot(AB, AB);

	Vector2 X = projAPonAB * AB + segA;
	Vector2 XP  = cirP - X; 
	float xp2 =  length2(XP);

	if( xp2 < radius2 )
	{
		float xp = sqrtf( xp2 );
		contactDepth =  radius - xp;
		if ( xp > epsilon )
			contactNormal = XP / xp;
		else
			contactNormal = Vector2(0.0f, 1.0f);
		contactPoint = X + contactNormal * capsule->radius;
		return true;
	}

	return false;
}

bool FindCirclesOverlap(const Circle *circleA, const Vector2 &posA,
						const Circle *circleB, const Vector2 &posB,
						Vector2 &contactNormal,
						Vector2 &contactPoint,
						float &contactDepth)
{
	Vector2 A = circleA->localPos + posA;
	Vector2 B = circleB->localPos + posB;
	float radius = circleA->radius + circleB->radius;
	float raidus2 = radius * radius;

	Vector2 BA = A - B;

	if ( dot(BA, BA) < raidus2 )
	{
		float ba = glm::length( BA );
		if ( ba > epsilon )
			contactNormal = BA / ba;
		else
			contactNormal = Vector2(0.0f, 1.0f);
		contactDepth = radius-ba;
		contactPoint = B + circleB->radius * contactNormal;
		return true;
	}
	return false;

}


bool FindOverlap(const Shape *shape1, const Vector2 &shape1Pos,
				 const Shape* shape2, const Vector2& shape2Pos,
				 Vector2 &contactNormal,
				 Vector2 &contactPoint,
				 float &contactDepth)
{
	if(shape1->GetType() == eCircle && shape2->GetType() == eCapsule)
	{
		return FindCircleCapsuleOverlap((Circle *)shape1, shape1Pos, (Capsule *)shape2, shape2Pos,
			contactNormal, contactPoint, contactDepth);
	}
	else if(shape1->GetType() == eCapsule && shape2->GetType() == eCircle)
	{
		bool ret = FindCircleCapsuleOverlap((Circle *)shape2, shape2Pos, (Capsule *)shape1, shape1Pos,
			contactNormal, contactPoint, contactDepth);
			contactPoint = contactPoint + contactNormal * contactDepth;
			contactNormal=-contactNormal;
			return ret;
	}
	else if(shape1->GetType() == eCircle && shape2->GetType() == eCircle)
	{
		return FindCirclesOverlap((Circle *)shape1, shape1Pos, (Circle *)shape2, shape2Pos, 
			contactNormal, contactPoint, contactDepth);
	}
	else if(shape1->GetType() == eCircle && shape2->GetType() == eAABox)
	{
		return FindCircleAABoxOverlap((Circle *)shape1, shape1Pos, (AABox *)shape2, shape2Pos, 
			contactNormal, contactPoint, contactDepth);
	}
	else if(shape1->GetType() == eAABox && shape2->GetType() == eCircle)
	{
		bool ret = FindCircleAABoxOverlap((Circle *)shape2, shape2Pos, (AABox *)shape1, shape1Pos, 
			contactNormal, contactPoint, contactDepth);
			contactPoint = contactPoint + contactNormal * contactDepth;
			contactNormal=-contactNormal;
			return ret;
	}
	else
	{
		assert(!"Bad shape types");
		return false;
	}
}

