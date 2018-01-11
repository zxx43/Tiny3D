/*
 * aabb.h
 *
 *  Created on: 2017-4-1
 *      Author: a
 */

#ifndef AABB_H_
#define AABB_H_

#include "boundingBox.h"
#include <vector>

class AABB: public BoundingBox {
private:
	VECTOR3D vertices[8];
public:
	float sizex, sizey, sizez;
	VECTOR3D minVertex, maxVertex;

private:
	bool vertexInsideCamera(const VECTOR3D& vertex,const Camera* camera);
	bool intersectsWidthRay(const VECTOR3D& origin,const VECTOR3D& dir,float maxDistance);
	bool cameraVertexInside(const VECTOR3D& vertex);
public:
	AABB(const VECTOR3D& min,const VECTOR3D& max);
	AABB(const VECTOR3D& pos,float sx,float sy,float sz);
	AABB(const AABB& rhs);
	virtual ~AABB();
	virtual AABB* clone();
	virtual bool checkWithCamera(Camera* camera);
	void update(const VECTOR3D& newMinVertex,const VECTOR3D& newMaxVertex);
	void update(float sx, float sy, float sz);
	virtual void update(const VECTOR3D& pos);
	virtual void merge(const std::vector<BoundingBox*>& others);
};


#endif /* AABB_H_ */
