#include <Windows.h>

#include "PlayerControls.h"
#include "Entity.h"

void PlayerGameModel::Think(float dt) 
{ 
	Entity *e = owner;

	Fixture *foot1 = owner->body.GetFixtureList();
	Fixture *foot2 = foot1->GetNext();
	Fixture *body = foot2->GetNext();

	if(GetAsyncKeyState(VK_LEFT))
	{
		foot1->surfaceVelocity = Vector2(speed, 0);
		foot2->surfaceVelocity = Vector2(speed, 0);
		owner->body.ApplyImpulse(Vector2(-speed/8.0f, 0));
		owner->render->flipX =  false;
	}
	else if(GetAsyncKeyState(VK_RIGHT))
	{
		foot1->surfaceVelocity = Vector2(-speed, 0);
		foot2->surfaceVelocity = Vector2(-speed, 0);
		owner->body.ApplyImpulse(Vector2(speed/8.0f, 0));
		owner->render->flipX = true;
	}
	else
	{
		foot1->surfaceVelocity = Vector2(0, 0);
		foot2->surfaceVelocity = Vector2(0, 0);
	}


	if ( GetAsyncKeyState(VK_DOWN) )
	{
		((Circle *)body->GetShape())->localPos = Vector2(0.0f, -0.5f);
	}
	else
	{
		((Circle *)body->GetShape())->localPos = Vector2(0.0f, 0.0f);
	}

	if(GetAsyncKeyState(VK_UP) && grounded && !jumping )
	{
		e->body.ApplyImpulse( 24.0f * speed * Vector2(0, 1.0f) );
		jumping = true;
	}
}
