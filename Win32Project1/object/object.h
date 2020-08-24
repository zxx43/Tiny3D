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
#include "../physics/dynamicWorld.h"

class Node;

class Object {
public:
	Node* parent;
	vec3 position; // Object local position
	vec3 size;
	mat4 translateMat, rotateMat, scaleMat; // Local transform
	Mesh* mesh;
	Mesh* meshMid;
	Mesh* meshLow;
	int material;
	Billboard* billboard;
	mat4 localTransformMatrix,normalMatrix; // Local transform
	mat4 transformMatrix,transformTransposed; // Global transform
	vec4 rotateQuat;
	vec4 boundInfo;
	float* transforms; // Global translate used in GPU
	buff* transformsFull; // Global transform used in GPU (translate, quat, bounding)
	BoundingBox* bounding; // Bounding box in world space
	vec3 localBoundPosition;
	bool genShadow;
	int detailLevel;
	vec3 shapeOffset;
	CollisionShape* collisionShape;
	CollisionObject* collisionObject;
	float mass;
public:
	Object();
	Object(const Object& rhs);
	virtual ~Object();
	virtual Object* clone()=0;
	void caculateLocalAABB(bool looseWidth,bool looseAll);
	void caculateCollisionShape(); // Caculate collision obb
	CollisionObject* initCollisionObject();
	void setShapeOffset(const vec3& off) { shapeOffset = off; }
	void removeCollisionObject();
	void initMatricesData();
	void updateLocalMatrices();
	virtual void vertexTransform()=0;
	virtual void normalTransform()=0;
	void bindMaterial(int mid);
	bool checkInCamera(Camera* camera);
	virtual void setPosition(float x, float y, float z) = 0;
	virtual void setRotation(float ax, float ay, float az) = 0;
	virtual void setSize(float sx, float sy, float sz) = 0;
	void setBillboard(float sx, float sy, int mid);
	void updateObjectTransform(bool translate, bool rotate);
	void setMass(float m) { mass = m; }
};


#endif /* OBJECT_H_ */
