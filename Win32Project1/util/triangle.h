#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "../maths/Maths.h"

class Triangle {
private:
	vec3 pa, pb, pc;
public:
	vec3 normal;
	float pd;
private:
	void caculateNormal();
public:
	Triangle(const vec3& a, const vec3& b, const vec3& c);
	~Triangle() {}
	bool pointIsIn(const vec3& point);
	bool pointIsIn(const vec2& point);
	float caculateY(const float x, const float z);
};

#endif