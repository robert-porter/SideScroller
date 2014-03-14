#pragma once

const float maxVelocity = 20.0f;
const float maxVelocitySquared = maxVelocity * maxVelocity;
//	A velocity threshold for elastic collisions. Any collision with a relative linear
//	velocity below this threshold will be treated as inelastic.
const float velocityThreshold = 0.2f;
// TODO : do more to encourage bodies to be within linear slop distance from each other to encourage 
// more contact persistance
const float linearSlop = 0.1f;
// The penetration percentage that gets corrected per iteration by the position solver, 0 means there is no position correction, 1 means that 100% of pentration 
// is corrected per iteration. This value should probably be less than 1 so we can maintain more persistance per contacts for better warm starting, since solving 
// on a velocity level gives more correct physics, and so the position solver is not dominated so much by the order of the contacts. 
const float softness = 0.5f;





/*
	not really tuning params
*/
const int MaxNumBodies = 500;

const int MaxNumContacts = 1000;