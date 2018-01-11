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

MATRIX4X4 perspective(float fovy,float aspect,float zNear,float zFar);

MATRIX4X4 ortho(float left, float right, float bottom, float top, float n, float f);

int project(float objX, float objY, float objZ,
		const float* model, const float* proj, const int* view,
		float* winX, float* winY, float* winZ);

MATRIX4X4 rotateX(float angle);

MATRIX4X4 rotateY(float angle);

MATRIX4X4 rotateZ(float angle);

MATRIX4X4 scale(float size);

MATRIX4X4 scaleX(float size);

MATRIX4X4 scaleY(float size);

MATRIX4X4 scaleZ(float size);

MATRIX4X4 translate(float tx,float ty,float tz);

VECTOR4D mul(const VECTOR4D& a,const VECTOR4D& b);

VECTOR3D mul(const VECTOR3D& a, const VECTOR3D& b);

inline float angleToRadian(float angle) {
	return angle*PI/180;
}

inline float radianToAngle(float radian) {
	return radian*180/PI;
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

bool CaculateIntersect(const Line& line, const Plane& plane, const float lineDistance, VECTOR3D& result);

#endif /* UTIL_H_ */
