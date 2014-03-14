#pragma once
#include "MathUtils.h"

class Fixture;

float			MixFriction(float friction1, float friction2);
float			MixRestitution(float restitution1, float restitution2);

class Contact {
public:
				Contact();
				Contact(Fixture *a, Fixture *b,	const Vector2 &norml, const Vector2 &pointB, float sep);
	void		PreStep();
	void		ApplyImpulse();
	void		SetFriction(float friction);
	float		GetFriction() const;
	void		SetRestitution(float restitution);
	float		GetRestitution() const;
	bool		operator == (const Contact &c) const;
	void		SetEnabled(bool en) { enabled = en; }
	void		PositionCorrection(float &maxOverlap);

	Fixture*	A;
	Fixture*	B;
	Vector2		normal;
	float		mixedFriction;
	float		mixedRestitution;
	float		normalImpulse;
	float		tangentImpulse;
	float		W; //  1 / ( 1/ma + 1/mb )
	float		bounce;
	Vector2		surfaceVel;
	Vector2		pointOnB;
	float		separation;
	bool		enabled;
	bool		persistant; // true if this contact existed the prev frame
	bool		bodyRemoved; // one of the bodies associated with this contact has been removed from Physics World. 
							 // If this is true the contact should be ignored. 
private:
	friend class PhysicsWorld;
	friend class ContactManager;
};

inline Contact::Contact() { 
}

inline float MixFriction(float friction1, float friction2) {
	return std::sqrt(friction1 * friction2);
}

inline float MixRestitution(float restitution1, float restitution2) {	
	return restitution1 > restitution2 ? restitution1 : restitution2;
}

inline void Contact::SetFriction(float friction)  { 
	mixedFriction = friction;
}

inline float Contact::GetFriction() const { 
	return mixedFriction;
}

inline void Contact::SetRestitution(float restitution) { 
	mixedRestitution = restitution; 
}

inline float Contact::GetRestitution() const { 
	return mixedRestitution;
}


