#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "../maths/Maths.h"

class Triangle {
private:
	VECTOR3D pa, pb, pc;
public:
	VECTOR3D normal;
	float pd;
private:
	void caculateNormal();
public:
	Triangle(const VECTOR3D& a, const VECTOR3D& b, const VECTOR3D& c);
	~Triangle() {}
	bool pointIsIn(const VECTOR3D& point);
	bool pointIsIn(const VECTOR2D& point);
	float caculateY(const float x, const float z);
};

#endif