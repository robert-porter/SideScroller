#pragma once
#include "Contact.h"
#include "TimeStep.h"
#include "TuningParams.h"

class Fixture;
class Body;
class ContactFilter;
class ContactListener;

// TODO : cache more than 1 frame?
class ContactManager {
public:
					ContactManager( ) :numContacts(0), contactFilter(0) { }
	void			AddPair(Fixture *A, Fixture *B);
	void			MergeContacts();
	ContactListener* contactListener;
	ContactFilter*	contactFilter;
	Contact			contacts[MaxNumContacts];
	Contact			oldContacts[MaxNumContacts];
	int				numContacts;
	int				numOldContacts;
};

class PhysicsWorld {
public:
					PhysicsWorld(); 
					~PhysicsWorld();
	void			AddBody(Body *pBody);
	void			RemoveBody(Body *pBody);
	void			Step(const TimeStep &ts, int velocityIterations, int positionIterations);
	Vector2			GetGravity( ) const;
	void			SetGravity(const Vector2 &newGravity);
	ContactManager	contactManager;
private:
	void			FindAllCollisions();// TODO: Broad phase 
	Vector2			gravity;
	int				numBodies;
	Body*			bodies[MaxNumBodies];
};

inline Vector2 PhysicsWorld::GetGravity( ) const {
	return gravity;
}

inline void PhysicsWorld::SetGravity(const Vector2 &newGravity) {
	gravity = newGravity;
}



