#include "triangle.h"

/*
		pa
		/\
	   /  \
	 pb----pc
*/
Triangle::Triangle(const vec3& a, const vec3& b, const vec3& c) {
	pa = a; pb = b; pc = c;
	caculateNormal();
	pd = -normal.DotProduct(pa);
}

void Triangle::caculateNormal() {
	vec3 ba = pa - pb;
	vec3 bc = pc - pb;
	normal = bc.CrossProduct(ba);
}

// Determine whether point in this triangle
bool Triangle::pointIsIn(const vec3& point) {
	vec3 v0 = pc - pb;
	vec3 v1 = pa - pb;
	vec3 v2 = point - pb;

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
bool Triangle::pointIsIn(const vec2& point) {
	vec2 v0 = vec2(pc.x, pc.z) - vec2(pb.x, pb.z);
	vec2 v1 = vec2(pa.x, pa.z) - vec2(pb.x, pb.z);
	vec2 v2 = point - vec2(pb.x, pb.z);

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