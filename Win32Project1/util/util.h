/*
 * util.h
 *
 *  Created on: 2014-8-17
 *      Author: a
 */

#ifndef UTIL_H_
#define UTIL_H_

#include "../maths/Maths.h"
#include "../constants/constants.h"
#include <stdio.h>

MATRIX4X4 lookAt(float eyeX, float eyeY, float eyeZ,
		float centerX, float centerY, float centerZ,
		float upX, float upY, float upZ);

MATRIX4X4 lookAt(const VECTOR3D& eye, const VECTOR3D& center, const VECTOR3D& up);

MATRIX4X4 perspective(float fovy,float aspect,float zNear,float zFar);

MATRIX4X4 ortho(float left, float right, float bottom, float top, float n, float f);

int project(float objX, float objY, float objZ,
		const float* model, const float* proj, const int* view,
		float* winX, float* winY, float* winZ);

MATRIX4X4 rotateX(float angle);

MATRIX4X4 rotateY(float angle);

MATRIX4X4 rotateZ(float angle);

MATRIX4X4 scale(float size);

MATRIX4X4 scale(float sx, float sy, float sz);

MATRIX4X4 scaleX(float size);

MATRIX4X4 scaleY(float size);

MATRIX4X4 scaleZ(float size);

MATRIX4X4 translate(float tx,float ty,float tz);

MATRIX4X4 translate(const VECTOR3D& t);

VECTOR4D mul(const VECTOR4D& a,const VECTOR4D& b);

VECTOR3D mul(const VECTOR3D& a, const VECTOR3D& b);

inline float angleToRadian(float angle) {
	return angle*A2R;
}

inline float radianToAngle(float radian) {
	return radian*R2A;
}

struct Line {
	VECTOR3D dir;
	VECTOR3D origin;
	Line(const VECTOR3D& ld, const VECTOR3D& lo) {
		dir = ld;
		origin = lo;
	}
};

struct Plane {
	VECTOR3D normal;
	float d;
	Plane() {}
	Plane(const VECTOR3D& pn, const float pd) {
		normal = pn;
		d = pd;
	}
	void update(const VECTOR3D& pn, const float pd) {
		normal = pn;
		d = pd;
	}
};

bool CaculateIntersect(const Line* line, const Plane* plane, const float lineDistance, VECTOR3D& result);

inline void RestrictAngle(float& angle) {
	if (angle > 360.0) angle -= 360.0;
	else if (angle < 0.0) angle += 360.0;
}

inline float GetVec2(const VECTOR2D* vec2, int i) {
	float ret;
	switch (i) {
		case 0:
			ret = vec2->GetX();
			break;
		case 1:
			ret = vec2->GetY();
			break;
	}
	return ret;
}

inline float GetVec3(const VECTOR3D* vec3, int i) {
	float ret;
	switch (i) {
		case 0:
			ret = vec3->GetX();
			break;
		case 1:
			ret = vec3->GetY();
			break;
		case 2:
			ret = vec3->GetZ();
			break;
	}
	return ret;
}

inline float GetVec4(const VECTOR4D* vec4, int i) {
	float ret;
	switch (i) {
		case 0:
			ret = vec4->GetX();
			break;
		case 1:
			ret = vec4->GetY();
			break;
		case 2:
			ret = vec4->GetZ();
			break;
		case 3:
			ret = vec4->GetW();
			break;
	}
	return ret;
}

#endif /* UTIL_H_ */
