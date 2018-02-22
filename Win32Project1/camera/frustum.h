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
public:
	VECTOR4D ndcVertex[8];
	VECTOR3D worldVertex[8];
	VECTOR3D normals[6];
	Plane planes[6];
	uint planeVertexIndex[24];
	float ds[6];
	VECTOR3D edgeDir[4];
	float edgeLength[4];

	Frustum();
	~Frustum();
	void update(const MATRIX4X4& invViewProjectMatrix, const VECTOR3D& lookDir);
	bool intersectsWidthRay(const VECTOR3D& origin, const VECTOR3D& dir, float maxDistance);
};

#endif /* FRUSTUM_H_ */
