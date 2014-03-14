#include "Body.h"



/*
	Calculating damping
	v(t) = v0 * e^(-kt)
	v(t+h) = v0 * e^(-k(t+h))
	v(t+h) = v0 * e^(-kt) * e^(-kh)
	v(t+h) = v(t) * e^(-kh)
	first order taylor expansion
	v(t+h) = v(t) * (1 - kh)
*/
void DefaultUpdateVelocity(Body *body, float dt,  const Vector2 &gravity) {
	if ( body->GetType() == eDynamic ) {
		body->velocity += ( gravity * body->gravityScale + body->force * body->invMass ) * dt;
		body->velocity *= Clamp(1.0f - dt * body->velocityDamping, 0.0f, 1.0f);		
	}
}

void DefaultUpdatePosition(Body *body, float dt, float maxTranslation) {
	//	Clamp max velocity
	Vector2 deltaPos = body->velocity * dt;
	float d2 = length2(deltaPos);
	if ( d2 > maxTranslation * maxTranslation ) {
		deltaPos *= maxTranslation / sqrtf(d2);
	}
	body->position += deltaPos;
}

void CircleVelocity(Body *body, float dt, const Vector2 &gravity) {
	body->velocity = Vector2(-body->position.y - 2.0f, body->position.x - 2.0f);
}

void Body::AttachFixture(const FixtureDef& def) {
	Fixture *fixture = new Fixture;
	fixture->shape = def.shape->Clone();
	fixture->body = this;
	fixture->contactMask = def.contactMask;
	fixture->density = def.density;
	fixture->friction = def.friction;
	fixture->isSensor = def.isSensor;
	fixture->restitution = def.restitution;
	fixture->surfaceVelocity = def.surfaceVelocity;
	fixture->next = fixtureList;
	fixtureList = fixture;
	fixtureCount++;
	ResetMass();
}

void Body::RemoveFixture(Fixture *fixture) {
	assert(fixture->body == this);
	assert(fixtureCount > 0);
	// Remove the fixture from this body's singly linked list.
	Fixture** node = &fixtureList;
	bool found = false;

	while (node != NULL) {
		if (*node == fixture) {
			*node = fixture->next;
			found = true;
			break;
		}
		node = &(*node)->next;
	}
	assert(found);	// fixture was not attached to the body
	delete *node;
	fixtureCount--;
	ResetMass();
}

void Body::ResetMass( ) {
	if ( type == eStatic || type == eKinematic ) {
		invMass = 0.0f;
		return;
	}
	if ( fixtureCount == 0 ) {
		return;
	}
	Fixture *f = fixtureList;
	int debugCount = 0;
	float mass = 0;
	while ( f ) {
		mass += f->CalculateMass();
		debugCount++;
		f = f->next;
	}
	assert(debugCount == fixtureCount);
	if ( fabs(mass) > 0.0001f ) {
		invMass = 1.0f / mass;
	} else { // HACK HACK
		assert(false && "Reset mass, mass is weird value");
		invMass = 0.0f;
	}
}