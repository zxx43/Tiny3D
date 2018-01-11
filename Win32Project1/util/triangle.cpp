#include "triangle.h"

/*
		pa
		/\
	   /  \
	 pb----pc
*/
Triangle::Triangle(const VECTOR3D& a, const VECTOR3D& b, const VECTOR3D& c) {
	pa = a; pb = b; pc = c;
	caculateNormal();
	pd = -normal.DotProduct(pa);
}

void Triangle::caculateNormal() {
	VECTOR3D ba = pa - pb;
	VECTOR3D bc = pc - pb;
	normal = bc.CrossProduct(ba);
}

// Determine whether point in this triangle
bool Triangle::pointIsIn(const VECTOR3D& point) {
	VECTOR3D v0 = pc - pb;
	VECTOR3D v1 = pa - pb;
	VECTOR3D v2 = point - pb;

	float dot00 = v0.DotProduct(v0);
	float dot01 = v0.DotProduct(v1);
	float dot02 = v0.DotProduct(v2);
	float dot11 = v1.DotProduct(v1);
	float dot12 = v1.DotProduct(v2);

	float inverDeno = 1.0f / (dot00 * dot11 - dot01 * dot01);

	float u = (dot11 * dot02 - dot01 * dot12) * inverDeno;
	if (u < 0 || u > 1) // if u out of range, return directly
		return false;

	float v = (dot00 * dot12 - dot01 * dot02) * inverDeno;
	if (v < 0 || v > 1) // if v out of range, return directly
		return false;

	return u + v <= 1;
}

// Determine whether point in this project triangle
bool Triangle::pointIsIn(const VECTOR2D& point) {
	VECTOR2D v0 = VECTOR2D(pc.x, pc.z) - VECTOR2D(pb.x, pb.z);
	VECTOR2D v1 = VECTOR2D(pa.x, pa.z) - VECTOR2D(pb.x, pb.z);
	VECTOR2D v2 = point - VECTOR2D(pb.x, pb.z);

	float dot00 = v0.x*v0.x + v0.y*v0.y;
	float dot01 = v0.x*v1.x + v0.y*v1.y;
	float dot02 = v0.x*v2.x + v0.y*v2.y;
	float dot11 = v1.x*v1.x + v1.y*v1.y;
	float dot12 = v1.x*v2.x + v1.y*v2.y;

	float inverDeno = 1.0f / (dot00 * dot11 - dot01 * dot01);

	float u = (dot11 * dot02 - dot01 * dot12) * inverDeno;
	if (u < 0 || u > 1) // if u out of range, return directly
		return false;

	float v = (dot00 * dot12 - dot01 * dot02) * inverDeno;
	if (v < 0 || v > 1) // if v out of range, return directly
		return false;

	return u + v <= 1;
}

float Triangle::caculateY(const float x, const float z) {
	float a = normal.x, b = normal.y, c = normal.z, d = pd;
	float y = -(d + a*x + c*z) / b;
	return y;
}