#include "CreateEntity.h"
#include "Entity.h"
#include "globals.h"

void UpDownVelocityFunc(Body *body, float dt, const Vector2 &gravity)
{
	float yMin = 6.0f;
	float yMax = 12.0f;

	if ( body->position.y < yMin )
	{
		body->velocity.y = 1.0f;
	}
	else if ( body->position.y > yMax )
	{
		body->velocity.y = -1.0f;
	}
}




Entity* CreateMainPlayer(float posX, float posY)
{
	Entity *e = new Entity;
	e->body.SetPosition(Vector2(posX, posY));

	/*
		set keyboard controller
	*/
	e->pGameModel = new PlayerGameModel(e);

	/*
		create body fixtures
	*/

	Circle bodyShape;
	bodyShape.radius = 1.0f;
	bodyShape.localPos = Vector2(0.0f, 0.0f);

	FixtureDef bodyFixtureDef;
	bodyFixtureDef.shape = &bodyShape;
	bodyFixtureDef.restitution =0.00f;
	bodyFixtureDef.friction = 0.9f;
	bodyFixtureDef.density = 1.2f;
	e->body.AttachFixture(bodyFixtureDef);

	/*
		create feet fixtures
	*/
	Circle footShape;
	footShape.radius = 0.2f;
	footShape.localPos = Vector2(0.5f, -1.5f * bodyShape.radius); // left foot

	FixtureDef footFixtureDef;
	footFixtureDef.shape = &footShape;
	footFixtureDef.friction = 1.9f;
	footFixtureDef.restitution = 0.00f;
	bodyFixtureDef.density = 4.2f;
	e->body.AttachFixture(footFixtureDef);

	footShape.localPos = Vector2(-.5f, -1.5f * bodyShape.radius); // right foot
	e->body.AttachFixture(footFixtureDef);

	e->body.SetType( eDynamic );

	TextureID ids[3];
	ids[0] = textures.anim;
	ids[1] = textures.gunAnim;
	ids[2] = textures.dude;

	e->render = new Animation(e, ids, 3, 0.2f);
	e->render->texture = textures.dude;

	return e;
}

Entity* CreateDude(float posX, float posY)
{
		Entity *e = new Entity;
	e->body.SetPosition(Vector2(posX, posY));



	/*
		create body fixtures
	*/

	Circle bodyShape;
	bodyShape.radius = 0.7f;
	bodyShape.localPos = Vector2(0.0f, 0.0f);

	FixtureDef bodyFixtureDef;
	bodyFixtureDef.shape = &bodyShape;
	bodyFixtureDef.restitution =  Random(0.0f, 1.0f);
	bodyFixtureDef.friction = Random(0.0f, 1.0f);
	bodyFixtureDef.density = 1.2f;
	e->body.AttachFixture(bodyFixtureDef);


	e->body.SetType( eDynamic );

	TextureID ids[3];
	ids[0] = textures.anim;
	ids[1] = textures.gunAnim;
	ids[2] = textures.dude;

	e->render = new Animation(e, ids, 3, 0.2f);
	e->render->texture = textures.dude;

	return e;
}

Entity* CreateBox(float posX, float posY, float width, float height)
{
	Entity *e = new Entity;
	e->body.SetPosition(Vector2(posX, posY));

	e->body.SetType( eStatic );

	AABox boxShape;
	boxShape.minExt = Vector2(0, 0);
	boxShape.maxExt = Vector2(width, height);

	FixtureDef bodyFixtureDef;
	bodyFixtureDef.shape = &boxShape;
	bodyFixtureDef.restitution = 0.00f;
	bodyFixtureDef.friction = 0.9f;
	e->body.AttachFixture(bodyFixtureDef);

	e->render = new Graphics(e);
	e->render->texture = rand()%2 ? textures.gunAnim : textures.dude;

	return e;
}

Entity *CreateUpDownMovingPlatform(float posX, float posY, float width, float height)
{
	Entity *e = CreateBox(posX, posY, width, height);

	e->body.SetType( eKinematic );
	e->body.UpdateVelocity = UpDownVelocityFunc;

	return e;
}

Entity *CreateUpDownMovingConveyerPlatform(float posX, float posY, float width, float height)
{
	Entity *e = CreateUpDownMovingPlatform(posX, posY, width, height);

	e->body.GetFixtureList()->surfaceVelocity = Vector2(3.0f, 0.0f);

	return e;
}


Entity* CreateOneWayPlatform(float posX, float posY, float width, float height)
{
	Entity *e = CreateBox(posX, posY, width, height);
	e->type = eOneWayPlatform;
	return e;
}


Entity * CreateSaw(float posX, float posY) 
{
	Entity *e = new Entity;

	e->body.SetPosition(Vector2(posX, posY));


	Circle circle;
	circle.radius = 3.8f;
	circle.localPos = Vector2(0.0f, 0.0f);

	FixtureDef bodyFixtureDef;
	bodyFixtureDef.shape = &circle;
	bodyFixtureDef.restitution =0.9f;
	bodyFixtureDef.friction = 0.2f;
	bodyFixtureDef.density = 1.2f;
	e->body.AttachFixture(bodyFixtureDef);

	e->body.SetType( eStatic );

	e->render = new SawGraphics(e);
	e->render->texture = textures.sawblade;

	e->type = eSaw;
	return e;
}
