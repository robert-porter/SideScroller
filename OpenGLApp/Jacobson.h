#pragma once
#include "MathUtils.h"

const int numParticles = 12;
const int numJoints = numParticles - 1;
const int numIterations = 1;

struct Particle {
	Vector2 x0;
	Vector2 x;
	Vector2 v;
};


struct DistanceJoint {
	Particle *p0, *p1;
	float len;
	int changeTheVectors;

	void Solve() {
		Vector2 dx = p1->x - p0->x;
		float dxLen = length(dx);
		Vector2 dir;
		if ( dxLen > 0.0001f ) {
			dir = dx / dxLen;
		} else { 
			dir = Vector2(0.0f, 1.0f);
		}
		float dif = dxLen - len;
		Vector2 P = (dir/2.0f) * dif;
		p0->x += P;
		p1->x -= P;
	}
};


const int nParticlesX = 12;
const int nParticlesY = 12;
const int nParticles = nParticlesX * nParticlesY;
const int nClothJoints = 10000; //2 * (nParticlesX) * (nParticlesY-1);

struct Cloth {
	inline void Init() {
	//	for(int i = 0; i < nClothJoints; i++)
	//		joints[i].id = i;

		for ( int i = 0; i < nParticlesX; i++ ) {
			for ( int j = 0; j < nParticlesY; j++ ) {
				particles[i *nParticlesX  + j].x = Vector2(float(i), float(-j));
				particles[i *nParticlesX  + j].x0 = Vector2(float(i), float(-j));
				particles[i *nParticlesX  + j].v = Vector2(0.0f, 0.0f);
			}
		}
		joint = 0;
		for ( int i = 0; i < nParticles; i++ ) {
			if ( (i+1)%nParticlesX && i != nParticles-1) {
				joints[joint].p0 = &particles[i];
				joints[joint].p1 = &particles[i+1];
				joints[joint].len = 1.0f;
				joint++;
			}
			if(i < nParticles - nParticlesX) {  
				joints[joint].len = 1.0f;
				joints[joint].p0 = &particles[i];
				joints[joint].p1 = &particles[i+nParticlesY];
				joint++;
			}
			if ( ((i+1)%nParticlesX && i != nParticles-1) && (i < nParticles - nParticlesX) ) {
				joints[joint].len = 1.0f;
				joints[joint].p0 = &particles[i];
				joints[joint].p1 = &particles[i+1+nParticlesY];
				joint++;
			}
		}
	//	joints[joint].len = sqrtf((nParticlesX-1) * (nParticlesX-1) + (nParticlesY-1) * (nParticlesY-1));
	//	joints[joint].p0 = &particles[0];
	//	joints[joint].p1 = &particles[nParticles-1];
	//	joint++;
		anchorPointA = Vector2(0.5f, 0.0f);
		anchorPointB = Vector2(0.0f , 0.0f);
	}
	inline void Step(const Vector2 &mouse, const float dt) {

		for ( int k = 0; k < numIterations; k++ ) {
			particles[0].x = mouse;
			particles[nParticlesX-1].x = anchorPointB;


			for ( int q = 0; q < joint; q++ ) {
				joints[q].Solve();
				particles[nParticlesX-1].x = anchorPointB;
			}
		}
		for ( int i = 0; i < nParticles; i++ ) {
			Vector2 tmp = particles[i].x;
			particles[i].x += 0.99f * ( particles[i].x - particles[i].x0 ) + Vector2(0.0f, -9.8f) * 0.5f * dt * dt;
			particles[i].x0 = tmp;
		}
	}
	Particle particles[nParticles];
	int joint;
	DistanceJoint joints[nClothJoints];

	Vector2 anchorPointA;
	Vector2 anchorPointB;
};

struct JWorld {
	Particle particles[numParticles];
	DistanceJoint joints[numJoints];
	inline void Init() {
		for ( int i = 0; i < numParticles; i++ ) {
			particles[i].x = Vector2(float(i), 0.0f);
		}
		for ( int i = 0; i < numJoints; i++ ) {
			joints[i].p0 = &particles[i];
			joints[i].p1 = &particles[i+1];
			joints[i].len = 1.0f;
		}
	}
	inline void Step(const float dt, const Vector2 &mouse) {
		Vector2 m = mouse;
		float mDis = length(mouse);
		if ( mDis > float(numJoints) ) {
			m = float(numJoints) * mouse / mDis;
		}
		for ( int k = 0; k < numIterations; k++ ) {
			particles[0].x = Vector2(0.0f, 0.0f);
			particles[numParticles-1].x = m;
			for ( int i = 0; i < numJoints; i++ ) {
				joints[i].Solve();
			}
		}
	}
};