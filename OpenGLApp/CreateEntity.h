#pragma once
#include "MathUtils.h"

class Body;
class Entity;

void UpDownVelocityFunc(Body *body, float dt, const Vector2 &gravity);

Entity* CreateMainPlayer(float posX, float posY);
Entity* CreateDude(float posX, float posY);
Entity* CreateBox(float posX, float posY, float width, float height);
Entity *CreateUpDownMovingPlatform(float posX, float posY, float width, float height);
Entity *CreateUpDownMovingConveyerPlatform(float posX, float posY, float width, float height);
Entity* CreateOneWayPlatform(float posX, float posY, float width, float height);
Entity * CreateSaw(float posX, float posY) ;