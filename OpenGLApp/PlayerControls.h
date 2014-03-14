#pragma once

class Entity;


/*
	if ( jumped )
		jumpTime = 0;

*/


class PlayerGameModel
{
public:
	PlayerGameModel(Entity *ownr) : owner(ownr), speed(6.0f), jumping(false) { }
	
	bool jumping;
	bool wasGrounded;
	bool grounded;

	void	Think(float dt);
	float	speed;
	Entity *owner;
};

