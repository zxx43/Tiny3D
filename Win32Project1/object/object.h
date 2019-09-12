/*
 * object.h
 *
 *  Created on: 2017-4-2
 *      Author: a
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include "../mesh/mesh.h"
#include "../material/materialManager.h"
#include "../billboard/billboard.h"
#include "../bounding/aabb.h"

class Object {
public:
	vec3 position;
	float sizex, sizey, sizez;
	Mesh* mesh;
	Mesh* meshMid;
	Mesh* meshLow;
	int material;
	Billboard* billboard;
	mat4 localTransformMatrix,normalMatrix;
	mat4 transformMatrix,transformTransposed;
	float* transforms;
	BoundingBox* bounding;
	vec3 localBoundPosition;
	bool genShadow;
	int detailLevel;

	Object();
	Object(const Object& rhs);
	virtual ~Object();
	virtual Object* clone()=0;
	virtual void caculateLocalAABB(bool looseWidth,bool looseAll);
	void updateLocalMatrices();
	virtual void vertexTransform()=0;
	virtual void normalTransform()=0;
	void bindMaterial(int mid);
	bool checkInCamera(Camera* camera);
	virtual void setPosition(float x, float y, float z) = 0;
	virtual void setRotation(float ax, float ay, float az) = 0;
	virtual void setSize(float sx, float sy, float sz) = 0;
	void setBillboard(float sx, float sy, int mid);
};


#endif /* OBJECT_H_ */
