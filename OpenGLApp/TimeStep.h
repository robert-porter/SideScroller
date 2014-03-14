#pragma once



struct TimeStep
{
	float t0;
	float t;
	float dt; //= t - t0
	float invDt;
};