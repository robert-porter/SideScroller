#pragma once

#include "Render.h"
#include "Body.h"
#include "PlayerControls.h"

enum EntityType 
{
	eNone,
	eMainPlayer,
	ePlatform,
	eOneWayPlatform, 
	eMovingUDPlatform,
	eMovingLRPlatform,
	eMovingCirclePlatform,
	eSaw,
	eMovingCircleSaw, 
	eNumEntityTypes
};

class Entity
{
public:
	Entity(void);
	
	virtual ~Entity();

	bool IsInvalid() const { return !valid;	}


	Body			 body;
	Graphics*		 render;
	PlayerGameModel* pGameModel;
	bool			 valid;			// set false when this object is destroyed
	EntityType		 type;
};

















/*

class Gun

{
public:
	Gun(void) : timeAccum(0.0f), firingDelay(1.0f), rotation(0.0f), angularVelocity(2.0f) { }

	void Update(float deltaTime)
	{
		timeAccum += deltaTime;
		rotation += angularVelocity * deltaTime;
	}

	bool ReadyToFire( ) const 
	{
		return timeAccum > firingDelay;
	}

	void ResetFiringTime( )
	{
		timeAccum = 0;
	}

	float rotation;
	float angularVelocity;
protected:
	float firingDelay;
	float timeAccum;

};


*/