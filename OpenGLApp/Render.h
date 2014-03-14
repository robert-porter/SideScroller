#pragma once


#include "MathUtils.h"
#include <map>
#include <iostream>
#include <vector>

class Entity;
class Renderer;

typedef int TextureID;


class Graphics 
{
public:
	Graphics(Entity *ownr) : owner(ownr), color(1.0f, 1.0f, 1.0f, 1.0f), flipX(false), flipY(false), angle(0.0f), visible(true) { }

	virtual void Draw(Renderer *renderer);
	virtual void Update(float dt) { }


	TextureID	texture;
	Color4f		color;
	Entity*		owner;
	bool		flipX;
	bool		flipY;
	int			drawShape;
	bool		visible;
	float		angle;
};

class SawGraphics : public Graphics
{
public:
	SawGraphics(Entity *ownr) : Graphics(ownr) { }
	void Update(float dt) {
		angle += 0.5f * dt;
		if ( angle > 2.0f * PI ) {
			angle -= 2.0f * PI;
		}
	}
};



class Animation : public Graphics
{
public:

	Animation(Entity *owner, TextureID *textures, unsigned int numFrames, float delay) : Graphics(owner)
	{
		textureIDs.reserve(numFrames);
		for(unsigned int i = 0; i < numFrames; i++)
		{
			textureIDs.push_back(textures[i]);
		}
		this->delay = delay;

		currentFrame = 0;
		timeAccum = 0.0f;
	}

	void Draw(Renderer *renderer);
	void Update(float deltaTime)
	{
		timeAccum += deltaTime;
		if(timeAccum > delay)
		{
			timeAccum -= delay;
			currentFrame = (currentFrame + 1) % textureIDs.size();
		}
	}

	void Reset()
	{
		timeAccum = 0.0f;
		currentFrame = 0;
	}
private:

	int currentFrame;
	int numFrames;
	float timeAccum;
	float delay;
	std::vector<TextureID> textureIDs;
};

class AnimationSet : public Graphics
{
public:
	AnimationSet(Entity *owner, Animation *animations, int numAnimations) : Graphics(owner)
	{
	}
	void Update(float dt)
	{
		currentAnimation->Update(dt);
	}
	void Draw(Renderer *renderer)
	{
		currentAnimation->Draw(renderer);
	}
	void SetAnimation(int anim)
	{
		std::map<int, Animation *>::iterator i = animations.find(anim);
		if(i == animations.end())
		{
			std::cout<<"Animation doesn't exist";
			return;
		}
		else 
			currentAnimation = i->second;
		
		currentAnimation->Reset();
	}
private:
	std::map<int, Animation *> animations;
	Animation *currentAnimation;
};

	/*
class Animation : public Graphics {


};

*/

