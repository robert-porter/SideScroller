
#include <amp_graphics.h>
#pragma once


#include "MathUtils.h"
#include "Fixture.h"

class Body;

/*
	Recomended that you overload both ( but not necessary )
	Failure to overload UpdateVelocity will give physically unrealistic collision response.
*/
class KinematicController {
public:
	virtual ~KinematicController() { }

	virtual void UpdateVelocity(Body *body, float dt, const Vector2 &gravity);
	virtual void UpdatePosition(Body *body, float dt, float maxTranslation);
};

typedef void(*UpdateVelocityFunc)(Body *body, float dt, const Vector2 &gravity);
//Try to avoid dispacements larger than maxTranslation
typedef void(*UpdatePositionFunc)(Body *body, float dt, float maxTranslation);


void		DefaultUpdateVelocity(Body *body, float dt, const Vector2 &gravity);
void		DefaultUpdatePosition(Body *body, float dt, float maxTranslation);
void		CircleVelocity(Body *body, float dt, const Vector2 &gravity);

class Entity;

enum BodyType { eStatic, eKinematic, eDynamic };
/*
	TODO : add a list of contact to body to build a contact graph
*/

class Body
{
public:
				Body(Entity *ownr);
	bool		IsValid() const { return VectorValid(position) && VectorValid(velocity) && VectorValid(force); }
	/*
		dynamic bodies cannot have 0 mass and 
		kinematic and static bodies always have 0 mass
	*/
	BodyType	GetType() const;
	void		SetType(BodyType _type);
	void		AttachFixture(const FixtureDef& def);
	void		RemoveFixture(Fixture *fixture);
	// Get the list of all fixtures attached to this body.
	Fixture*	GetFixtureList();
	const Fixture* GetFixtureList() const;
	Vector2		GetPosition() const;
	void		SetPosition(const Vector2 &pos);
	Vector2		GetVelocity() const;
	void		SetVelocity(const Vector2 &vel);
	void		ApplyImpulse(const Vector2 &imp);
	void		ApplyForce(const Vector2 &f);
	void		ResetMass();
	UpdateVelocityFunc UpdateVelocity;
	UpdatePositionFunc UpdatePosition;

	Entity*		owner;
	BodyType	type;
	float		gravityScale;
	float		velocityDamping;
	Vector2		position;
	Vector2		velocity;
	Vector2		force;
	Fixture*	fixtureList;
	int			fixtureCount;
	float		invMass;

	friend class PhysicsWorld;
	friend class Contact;
	friend class Fixture;
	friend class ContactManager;
};


/*
	Body inlines 
*/
inline Body::Body(Entity *ownr) { 
	fixtureList = 0;
	fixtureCount = 0;
	owner = ownr;
	position = Vector2(0.0f, 0.0f); 
	force = Vector2(0.0f, 0.0f); 
	velocity = Vector2(0.0f, 0.0f);
	invMass = 0.0f;
	velocityDamping = 0.05f;
	gravityScale = 1.0f;
	type = eStatic;;
	UpdateVelocity = DefaultUpdateVelocity;
	UpdatePosition = DefaultUpdatePosition;
}
	
inline BodyType Body::GetType() const {
	return type;
}

inline void Body::SetType(BodyType _type) {
	if ( type == _type ) {
		return;
	}
	type = _type;
	ResetMass();
	
	if ( _type == eStatic )	{
		velocity = Vector2(0.0f, 0.0f);
	}
	force = Vector2(0.0f, 0.0f);
}

inline Vector2 Body::GetPosition( ) const {
	return position;
}

inline void Body::SetPosition(const Vector2 &pos) {
	position = pos;
}
	
inline Vector2 Body::GetVelocity( ) const {
	assert(type != eStatic || velocity == Vector2(0.0f, 0.0f));
	return velocity;
}

inline void Body::SetVelocity(const Vector2 &vel) {
	assert(type != eStatic);
	velocity = vel;
}

inline void Body::ApplyImpulse(const Vector2 &imp) {
	velocity += imp * invMass;
}

inline void Body::ApplyForce(const Vector2 &f) {
	assert(type == eDynamic);
	force+=f;
}

inline Fixture* Body::GetFixtureList() {
	return fixtureList;
}

inline const Fixture* Body::GetFixtureList() const {
	return fixtureList;
}