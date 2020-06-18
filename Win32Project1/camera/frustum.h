/*
 * frustum.h
 *
 *  Created on: 2017-4-1
 *      Author: a
 */

#ifndef FRUSTUM_H_
#define FRUSTUM_H_

#include "../maths/Maths.h"
#include "../util/util.h"

class Frustum {
private:
	vec4 ndcVertex[8];
	Plane planes[6];
	static uint PlaneVertexIndex[24];
public:
	vec3 worldVertex[8];
	vec3 normals[6];
	float ds[6];
	vec3 edgeDir[4];
	float edgeLength[4];
public:
	Frustum();
	~Frustum();
	void update(const mat4& invViewProjectMatrix, const vec3& lookDir);
	bool intersectsWidthRay(const vec3& origin, const vec3& dir, float maxDistance);
};

#endif /* FRUSTUM_H_ */
