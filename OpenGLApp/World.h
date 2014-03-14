#pragma once
#include <vector>

#include "TileMap.h"
#include "PhysicsWorld.h"
#include "Entity.h"
#include "globals.h"
#include "ContactFilter.h"

class Entity;


class World : public ContactListener
{
public:
	World() :  initPos(0, 1)
	{ 
		physicsWorld.SetGravity(Vector2(0.0f, -22.0f));
		physicsWorld.contactManager.contactListener = this;

	}

	Entity *dude;


	void OnContact(Contact* contact)
	{

		if ( ( contact->A->GetBody()->owner->type == eSaw && contact->B->GetBody()->owner == dude ) ||
			( contact->B->GetBody()->owner->type == eSaw && contact->A->GetBody()->owner == dude ) )
		{

			dude->body.position = initPos;
			contact->SetEnabled(false);
		}

	}

	void PreSolve(Contact *contact)
	{


		if ( contact->A->GetBody()->owner->type == eOneWayPlatform &&  contact->B->GetBody()->owner == dude) {
			AABox *box = (AABox*)contact->A->GetShape();
			Circle *cir =  (Circle*)contact->B->GetShape();
			float boxTop = box->maxExt.y + contact->A->body->GetPosition().y;
			float bodyBottom = dude->body.GetPosition().y + cir->localPos.y - cir->radius;
			if (   dude->body.GetVelocity().y > 0.0f ) {// contact->normal.y > 0.0f )	{
				contact->SetEnabled(false);
			}
		} else if ( contact->A->GetBody()->owner == dude && contact->B->GetBody()->owner->type == eOneWayPlatform) {
			AABox *box = (AABox*)contact->B->GetShape();
			Circle *cir = (Circle*)contact->A->GetShape();
			float boxTop = box->maxExt.y + contact->B->body->GetPosition().y;
			float bodyBottom = dude->body.GetPosition().y + cir->localPos.y - cir->radius;
			if ( dude->body.GetVelocity().y > 0.0f  ) {// contact->normal.y > 0.0f )	{
				contact->SetEnabled(false);
			}
			//if ( contact->normal.y < 0.0f ) {
			//	contact->SetEnabled(false);
			//}
		}

		if ( contact->A->GetBody()->owner == dude ) {
			if ( contact->A == dude->body.GetFixtureList()  || contact->A == dude->body.GetFixtureList()->GetNext() ) {
				if ( contact->normal.y > 0.0f ) { 
					dude->pGameModel->grounded = true;
				}
			}
		} else if ( contact->B->GetBody()->owner == dude) {
			if ( contact->B == dude->body.GetFixtureList()  || contact->B == dude->body.GetFixtureList()->GetNext() ) {
				if ( contact->normal.y < 0.0f ) { 
					dude->pGameModel->grounded = true;
				}
			}
		}


	}


	void Update(const TimeStep &ts);

	void AddEntity(Entity *pEntity);
	
	void RemoveEntity(Entity *pEntity);
	
	void GarbageCollection();
	
	PhysicsWorld physicsWorld;

	Fixture *QueryPoint(const Vector2& point) const;

	TileMap tileMap;
	std::vector<Entity *> entities;
	Vector2 initPos;
};
