#include "Contact.h"
#include "Body.h"
#include "TuningParams.h"

Contact::Contact(Fixture *a, Fixture *b, 
				 const Vector2 &norml, const Vector2 &pointB, float sep) {
	assert(a && b);
	A = a;
	B = b;
	normal = norml;
	separation = sep;
	pointOnB = pointB;

	mixedFriction = MixFriction(a->GetFriction(), b->GetFriction());
	mixedRestitution = MixRestitution(a->GetRestitution(), b->GetRestitution());
	assert(mixedFriction >= 0.0f && mixedFriction <= 10.0f);
	assert(mixedRestitution >= 0.0f && mixedRestitution <= 1.0f);

	float M = A->GetBody()->invMass + B->GetBody()->invMass;
	assert(M > 0.0001f);
	W = 1.0f / M;

	surfaceVel = B->surfaceVelocity - A->surfaceVelocity;
	Vector2 dv = B->GetBody()->velocity - A->GetBody()->velocity;
	float vn = dot(dv, normal);  // only add surface vel to tangent.
	bounce = ( vn > velocityThreshold ) ? mixedRestitution * vn : 0.0f;

	normalImpulse = 0.0f;
	tangentImpulse = 0.0f;

	enabled = true;
	persistant = false;
	bodyRemoved = false;
}

void Contact::PreStep( ) {
	assert(!bodyRemoved);
	Vector2 tangent = Vector2(-normal.y, normal.x);
	Vector2 impulse = normalImpulse * normal + tangentImpulse * tangent;
	//	Apply accumulated impulses for warm starting
	A->GetBody()->velocity += impulse * A->GetBody()->invMass;	
	B->GetBody()->velocity -= impulse * B->GetBody()->invMass;
}

bool Contact::operator == (const Contact &c) const  {
	return ( A == c.A && B == c.B ) || ( A == c.B && B == c.A );
}

void Contact::ApplyImpulse() {
	assert(!bodyRemoved);
	Vector2 tangent = Vector2(-normal.y, normal.x);

	Vector2 dv = B->GetBody()->velocity - A->GetBody()->velocity;
	float vn = dot(dv, normal);
	float vt = dot(dv + surfaceVel, tangent);

	float lambdaN =  (vn + bounce/* + bias*/) * W;
	//Clamp accumulated normal impulse
	float normalImpulse0 = normalImpulse;
	normalImpulse = Max(normalImpulse0 + lambdaN, 0.0f);
	lambdaN = normalImpulse - normalImpulse0;

	float lambdaT = vt * W;
	//Clamp accumulated tangent impulse
	float maxFrictionImpulse = normalImpulse * mixedFriction;
	float tangentImpulse0 = tangentImpulse;
	tangentImpulse = Clamp(tangentImpulse0 + lambdaT, -maxFrictionImpulse, maxFrictionImpulse);
	lambdaT = tangentImpulse - tangentImpulse0;

	Vector2 impulse = lambdaN * normal + lambdaT * tangent;
	A->GetBody()->velocity += impulse * A->GetBody()->invMass;	
	B->GetBody()->velocity -= impulse * B->GetBody()->invMass;
}



void Contact::PositionCorrection(float &maxOverlap) {
	assert(!bodyRemoved);
	if ( FindOverlap(A->GetShape(), A->GetBody()->GetPosition(), 
					 B->GetShape(), B->GetBody()->GetPosition(),
					 normal, pointOnB, separation) ) {
		float impulse = separation * W * softness;
		assert(impulse >= 0.0f );
		Vector2 P = impulse * normal;
		A->GetBody()->position += P * A->GetBody()->invMass;	
		B->GetBody()->position -= P * B->GetBody()->invMass;
		if ( separation > maxOverlap ) {
			maxOverlap = separation;
		}
	}
}

/*
	TODO: find out if I can use integrated collision points for position correction rather
	than calling collision detection again.

void PositionCorrection() 
{
	for ( all contacts ) { 
		Vector2 dif = collPointB - collPointA;
		if ( dot(dif, normal) < 0.0f ) {
			float dis = length(dif);

			A += dis * W * invMassA * normal;
			collPointA += dis * W * invMassA * normal * softness;
			B -= dis * W * invMassB * normal;
			collPointB -= dis * W * invMassB * normal * softness;
		}
	}
}
*/
