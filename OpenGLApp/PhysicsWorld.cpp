#include "PhysicsWorld.h"
#include "Body.h"
#include "ContactFilter.h"
#include <memory>
#include <list>

void ContactManager::AddPair(Fixture *A, Fixture *B) {	
	Vector2 pt, norml;
	float sep;
	if ( FindOverlap(A->GetShape(), A->GetBody()->position, B->GetShape(), B->GetBody()->position, norml, pt, sep) ) {
		assert( A->GetBody() !=  B->GetBody() );
		contacts[numContacts] = Contact(A, B, norml, pt, sep);
		numContacts++;
		assert(numContacts < MaxNumContacts);
	}
}
/*
	Loop throught all the new contacts and look for a matching contact in the old
	contact list. If the new contact has a matching old contact then set the impulses for warm starting.
	If an old contact is not found the the contact is truely new and we can call the begin contact callback
*/ 	
//TODO: use hash map
void ContactManager::MergeContacts( ) {
	for ( int i = 0; i < numContacts; i++ )	{
		bool found = false;
		for ( int j = 0; j < numOldContacts; j++ ) {
			if ( oldContacts[j].bodyRemoved ) {
				continue;
			} else if ( oldContacts[j] == contacts[i] ) {
				contacts[i].normalImpulse = oldContacts[j].normalImpulse;
				contacts[i].tangentImpulse = oldContacts[j].tangentImpulse;
			//	contacts[i].enabled = oldContacts[j].enabled; // do this?
				oldContacts[j].persistant = true;
				found = true;
				break;
			}
		}
		if ( !found ) {
			contactListener->OnContact(&contacts[i]);
		}
	}
	for ( int i = 0; i < numOldContacts; i++ ) {
		if ( !oldContacts[i].persistant && !oldContacts[i].bodyRemoved ) {
			contactListener->OnSeperation(&oldContacts[i]);
		}
	}
}

PhysicsWorld::PhysicsWorld( ) {
	numBodies = 0;
}

PhysicsWorld::~PhysicsWorld( ) {
}

void PhysicsWorld::AddBody(Body *pBody) {
	assert(numBodies < MaxNumBodies);
	bodies[numBodies] = pBody;
	numBodies++;
}

void PhysicsWorld::RemoveBody(Body *pBody) {
	int index = 0; 
	for ( ; index < numBodies; index++ ) {
		if ( bodies[index] == pBody ) {
			break;
		}
	}
	if ( index == numBodies ) {
		assert(false && "RemoveBodies : Body not found");
	}
	//	remove all contacts associated with this body
	for ( int i = 0; i < contactManager.numContacts; i++ ) {
		if ( ( contactManager.contacts[i].A->GetBody() == pBody ) ||
			 ( contactManager.contacts[i].B->GetBody() == pBody ) )	{
			contactManager.contactListener->OnSeperation(&contactManager.contacts[i]);
			contactManager.contacts[i].bodyRemoved = true;
		}
	}
	if ( numBodies > 1 ) {
		std::swap(bodies[numBodies - 1], bodies[index]);
	}
	numBodies--;
	assert(numBodies >= 0);
}


void PhysicsWorld::Step(const TimeStep &ts, int velocityIterations, int positionIterations) {
	// TODO: POSITION CORRECTION FIRST??? HOW TO DO THIS
	FindAllCollisions();
	for ( int i = 0; i < numBodies; i++ ) {
		if ( !bodies[i]->type == eStatic ) {
			bodies[i]->UpdateVelocity(bodies[i], ts.dt, gravity);	
		}
	}
	for ( int i = 0; i < contactManager.numContacts; i++ ) {
		if ( contactManager.contacts[i].enabled ) {
			contactManager.contactListener->PreSolve(&contactManager.contacts[i]);
		}
	}
	for ( int i = 0; i < contactManager.numContacts; i++ ) {
		if ( contactManager.contacts[i].enabled ) {
			contactManager.contacts[i].PreStep();	
		}
	}
	for ( int k = 0; k < velocityIterations; k++ ) {
		for ( int i = 0; i < contactManager.numContacts; i++ ) {
			if ( contactManager.contacts[i].enabled ) {
				contactManager.contacts[i].ApplyImpulse();
			}
		}
	}
	for ( int i = 0; i < contactManager.numContacts; i++ ) {
		if ( contactManager.contacts[i].enabled ) {
			contactManager.contactListener->PostSolve(&contactManager.contacts[i]);
		}
	}
	for ( int i = 0; i < numBodies; i++ ) {
		if ( !bodies[i]->type == eStatic ) {
			bodies[i]->UpdatePosition(bodies[i], ts.dt, maxVelocity * ts.dt);
		}
	}
	for ( int k = 0; k < positionIterations; k++ ) {
		float maxOverlap = linearSlop * 2.0f;
		for ( int i = 0; i < contactManager.numContacts; i++ ) {
			if ( contactManager.contacts[i].enabled ) {
				contactManager.contacts[i].PositionCorrection(maxOverlap);
			}
		}
		if ( maxOverlap < linearSlop * 2.0f ) break;
	}
	for ( int i = 0; i < numBodies; i++ ) {
		if ( bodies[i]->type == eDynamic ) {
			bodies[i]->force = Vector2(0.0f, 0.0f);
		} else if ( bodies[i]->force != Vector2(0.0f, 0.0f) ) {
			assert(false && "Something is assigning forces to non dynamic bodies");
		}
	}
	for ( int i = 0; i < numBodies; i++ ) {
		assert(bodies[i]->IsValid()); // sanity check
	}
}

void PhysicsWorld::FindAllCollisions() {
	contactManager.numOldContacts = contactManager.numContacts;
	assert(contactManager.numOldContacts < MaxNumContacts );
	memcpy(contactManager.oldContacts, contactManager.contacts, contactManager.numOldContacts * sizeof(Contact));
	contactManager.numContacts = 0;
	/*
		O(n^2) collsion detection ( or worse LOL )
		Check all fixtures vs all fixtures for collisions, 
		We dont collide fixtures that are from the same body
	*/ 
	for ( size_t i = 0; i < numBodies; i++ ) {
		for ( size_t j = i + 1; j < numBodies; j++ ) {
			if ( (bodies[i]->type != eDynamic && bodies[j]->type != eDynamic) ) {
				continue;
			}
			if ( contactManager.contactFilter && !contactManager.contactFilter->ShouldCollide(bodies[i], bodies[j]) ) {
				continue;
			}
			Fixture *fi = bodies[i]->GetFixtureList();
			while ( fi ) {
				Fixture *fj = bodies[j]->GetFixtureList();
				while ( fj ) {
					if ( fi->IsSensor( ) || fj->IsSensor( ) ) {
						if ( CheckOverlap(fi->GetShape(), bodies[i]->GetPosition(), fj->GetShape(), bodies[j]->GetPosition()) )	{
							contactManager.contactListener->OnSensor(fi, fj);
						}
					} else {
						// check collision and add to the contact list if they intersect
						contactManager.AddPair(fi, fj);
					}
					fj = fj->GetNext();
				}
				fi = fi->GetNext();
			}
		}
	}
	contactManager.MergeContacts();
}		

		
