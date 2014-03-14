#pragma once

#include <math.h>
#include <float.h>
#include <assert.h>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>


const float PI = 3.141592653589793238462f;
const float DegToRad = PI / 180.0f;
const float RadToDeg = 180.0f / PI;

typedef glm::vec2 Vector2;
typedef glm::vec4 Color4f;
using glm::dot;
using glm::length;
using glm::length2;
using glm::normalize;


inline
bool FloatValid(const float f) {
	if ( f == f ) {  // NaN Check
		if ( f >= -FLT_MAX && f <= FLT_MAX ) { 
			return true;
		}
	}
	return false;
}

inline
bool VectorValid(const Vector2 &v) {
	return FloatValid(v.x) && FloatValid(v.y);
}

struct AABB {
	Vector2 minExt;
	Vector2 maxExt;
};

// finds the squared distance from the point P to the Segment P0P1
inline
float DistancePointSegmentSq(const Vector2 &P, const Vector2 &A, const Vector2 &B) {

	Vector2 AB = B-A;
	Vector2 AP = P-A;

	 // closest to A
	if( dot(AB, AP) < 0.0f ) { 
		return length2(AP);
	}

	Vector2 BP = P-B;
	Vector2 BA = -AB;

	// closest to B
	if( dot(BA, BP) < 0.0f ) {
		return length2(BP);
	}

	float projAPonAB = dot(AB, AP) / length2(AB);
	Vector2 X = A + projAPonAB * AB;
	Vector2 PX = P-X;

	return length2(PX);  // closest to a point on the interior of the segment
}


inline 
float Sign(const float x) {
	if ( x == 0.0f ) {
		return 0.0f;
	} else {
		return x < 0.0f ? -1.0f : 1.0f;
	}
}

template<class Type>
Type Min(const Type &a, const Type &b) {
	return a < b ? a : b;
}

template<class Type>
Type Max(const Type& a, const Type &b) {
	return a > b ? a : b;
}

template<class Type>
Type Clamp(const Type &val, const Type &low, const Type &high) {
	return Max(low, Min(val, high));
}

template<typename Type> 
void Swap(Type& a, Type& b) {
	Type tmp = a;
	a = b;
	b = tmp;
}

// Random number in range [-1,1]
inline 
float Random() {
	float r = (float)rand();
	r /= RAND_MAX;
	r = 2.0f * r - 1.0f;
	return r;
}

inline 
float Random(const float lo, const float hi) {
	float r = (float)rand();
	r /= RAND_MAX;
	r = (hi - lo) * r + lo;
	return r;
}
