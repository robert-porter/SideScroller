#include "Entity.h"
#include "World.h"
#include "Circle.h"
#include "Capsule.h"
#include "Renderer.h"
#include <iostream>
#include <chrono>

Entity::Entity(void) : body(this)
{
	body.SetPosition( Vector2(0,0) );
	render = 0;
	valid = true;
	pGameModel = NULL;
	type = eNone;
}

Entity::~Entity(void)
{
	if ( pGameModel )
		delete pGameModel;
}



