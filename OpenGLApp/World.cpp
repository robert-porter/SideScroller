#include "World.h"
#include "Entity.h"


void World::AddEntity(Entity *pEntity)
{ 
	entities.push_back(pEntity); 
	physicsWorld.AddBody(&pEntity->body);

}  

void World::Update(const TimeStep &ts)
{
	dude->pGameModel->Think(ts.dt);
	dude->pGameModel->wasGrounded = dude->pGameModel->grounded;
	dude->pGameModel->grounded = false;

	physicsWorld.Step(ts, 8, 3);

	
	if ( !dude->pGameModel->wasGrounded && dude->pGameModel->grounded )
		dude->pGameModel->jumping = false;
}

void World::RemoveEntity(Entity *pEntity)
{
	/*
	for(size_t i = 1; i < entities.size();)
	{
		if(entities[i] == pEntity)
		{
			Entity * p = entities[i];
			assert( p );
			entities[i] = entities[entities.size() - 1];
			physicsWorld.RemoveBody(&p->body);
			delete p;
			entities.pop_back();
		}
		else
			i++;
	}
	*/
	pEntity->valid = false; // This should work??
}

void World::GarbageCollection()
{
	for(size_t i = 1; i < entities.size();)
	{
		if(entities[i]->IsInvalid() )
		{
			Entity * p = entities[i];
			assert( p );
			entities[i] = entities[entities.size() - 1];
			physicsWorld.RemoveBody(&p->body);
			if ( p->pGameModel )
				delete p->pGameModel;
			if ( p->render )
				delete p->render;
			delete p;
			entities.pop_back();
		}
		else
			i++;
	}
}

Fixture * World::QueryPoint(const Vector2 &point) const
{
	for(int i = 0; i < entities.size(); i++)
	{
		for(Fixture *f = entities[i]->body.GetFixtureList(); f != 0; f = f->GetNext()) 
		{
			if(f->GetShape()->TestPoint(point, entities[i]->body.GetPosition()))
			{
				return f;
			}
		}
	}
	return 0;
}