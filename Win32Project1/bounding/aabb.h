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

struct BoxInfo {
	vec3 minPos, maxPos;
	BoxInfo(const vec3& min, const vec3& max) : minPos(min), maxPos(max) {}
	BoxInfo* clone() {
		return new BoxInfo(minPos, maxPos);
	}
	void update(const vec3& min, const vec3& max) {
		minPos = min;
		maxPos = max;
	}
};

class Node;
class AABB: public BoundingBox {
private:
	vec3 vertices[8];
public:
	float sizex, sizey, sizez;
	float radius;
	vec3 halfSize;
	vec3 minVertex, maxVertex;
	Node* debugNode;
private:
	bool vertexInsideCamera(const vec3& vertex, const Frustum* frustum);
	bool intersectsWidthRay(const vec3& origin,const vec3& dir,float maxDistance);
	bool cameraVertexInside(const vec3& vertex);
	void caculateRadius();
public:
	AABB(const vec3& min,const vec3& max);
	AABB(const vec3& pos,float sx,float sy,float sz);
	AABB(const AABB& rhs);
	virtual ~AABB();
	virtual AABB* clone();
	virtual bool checkWithCamera(Frustum* frustum, int checkLevel);
	virtual bool sphereWithCamera(Frustum* frustum);
	void update(const vec3& newMinVertex,const vec3& newMaxVertex);
	void update(float sx, float sy, float sz);
	void update(const mat4& mat);
	virtual void update(const vec3& pos);
	virtual void merge(const std::vector<BoundingBox*>& others);
};


#endif /* AABB_H_ */
