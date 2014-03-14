#pragma once
#include "Shape.h"

class Body;

struct ContactMask {
				ContactMask() : category(0x0), mask(0x0) { }
	void		AddCategory(int c) { category |= c; }
	void		AddCollider(int c) { mask |= c; }
	int			category;
	int			mask;
};

struct FixtureDef {
				FixtureDef();
	Shape*		shape;
	float		friction;
	float		restitution;
	ContactMask contactMask;
	float		density;	// in kg/meters^2
	bool		isSensor;
	Vector2		surfaceVelocity;
};

class Fixture {
public:
				Fixture();
				~Fixture();
	// Get the parent body of this fixture. This is NULL if the fixture is not attached.
	Body*		GetBody();
	const Body* GetBody() const;
	// Get the next fixture in the parent body's fixture list.
	Fixture*	GetNext();	
	const Fixture* GetNext() const;
	ShapeType	GetType() const;
	Shape*		GetShape();
	const Shape* GetShape() const;
	bool		IsSensor() const;
	void		SetDensity(float density);
	float		GetDensity() const;
	float		GetFriction() const;
	void		SetFriction(float _friction);
	float		GetRestitution() const;
	void		SetRestitution(float _restitution);
	bool		TestPoint(const Vector2& point) const;
	float		CalculateMass() const;
	void		GetAABB(AABB& aabb) const;

	Vector2		surfaceVelocity;
	Body*		body;
private:
	friend class Body;
	Shape*		shape;
	Fixture*	next;
	float		friction;
	float		restitution;
	ContactMask contactMask;
	float		density;	// in kg/meters^2
	bool		isSensor;
};

inline FixtureDef::FixtureDef( ) {
	shape = NULL;
	friction = 0.4f;
	restitution = 0.2f;
	density = 10.0f;
	isSensor = false;
	surfaceVelocity = Vector2(0.0f, 0.0f);
}
/*
	Fixture inlines 
*/
inline Fixture::Fixture() {
	shape = 0;
	body = 0;
	next = 0;
	friction = 0.8f;
	restitution = 0.8f;
	density = 10.0f;
	surfaceVelocity = Vector2(0.0f, 0.0f);
}

inline Fixture::~Fixture() {
	if ( shape )
		delete shape;
}

inline ShapeType Fixture::GetType() const {
	return shape->GetType();
}

inline Shape* Fixture::GetShape() {
	return shape;
}

inline const Shape* Fixture::GetShape() const {
	return shape;
}

inline bool Fixture::IsSensor() const {
	return isSensor;
}

inline Body* Fixture::GetBody() {
	return body;
}

inline const Body* Fixture::GetBody() const {
	return body;
}

inline Fixture* Fixture::GetNext() {
	return next;
}

inline const Fixture* Fixture::GetNext() const {
	return next;
}

inline void Fixture::SetDensity(float _density) {
	assert(density < 10000.0f && density >= 0.001f);
	density = _density;
}

inline float Fixture::GetDensity() const {
	return density;
}

inline float Fixture::GetFriction() const {
	return friction;
}

inline void Fixture::SetFriction(float _friction) {
	friction = _friction;
}

inline float Fixture::GetRestitution() const {
	return restitution;
}

inline void Fixture::SetRestitution(float _restitution) {
	restitution = _restitution;
}

inline float Fixture::CalculateMass() const {
	return shape->Area() * density;
}

