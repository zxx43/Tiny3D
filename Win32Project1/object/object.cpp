#include "object.h"
#include <stdlib.h>
#include "../node/node.h"
#include "../constants/constants.h"

Object::Object() {
	parent = NULL;
	position = vec3(0, 0, 0);
	size = vec3(1.0, 1.0, 1.0);
	localTransformMatrix.LoadIdentity();
	normalMatrix.LoadIdentity();

	translateMat.LoadIdentity();
	rotateMat.LoadIdentity();
	scaleMat.LoadIdentity();

	mesh = NULL;
	meshMid = NULL;
	meshLow = NULL;
	bounding = NULL;
	material = -1;
	localBoundPosition.x=0;
	localBoundPosition.y=0;
	localBoundPosition.z=0;

	billboard = NULL;
	genShadow = true;
	detailLevel = 2;

	transforms = NULL;
	transformsFull = NULL;
	rotateQuat = MatrixToQuat(rotateMat);
	boundInfo = vec4(0.0, 0.0, 0.0, 0.0);
}

Object::Object(const Object& rhs) {
	parent = rhs.parent;
	if (rhs.billboard)
		billboard = new Billboard(rhs.billboard->data[0], rhs.billboard->data[1], rhs.billboard->material);
	else
		billboard = NULL;
	transforms = NULL;
	transformsFull = NULL;
}

Object::~Object() {
	if (bounding) delete bounding; bounding = NULL;
	if (billboard) delete billboard; billboard = NULL;

	if (transforms) free(transforms); transforms = NULL;
	if (transformsFull) free(transformsFull); transformsFull = NULL;
	// todo remove collision object
}

void Object::initMatricesData() {
	transforms = (float*)malloc(4 * sizeof(float));
	transformsFull = (buff*)malloc(16 * sizeof(buff));

	transforms[0] = 0.0, transforms[1] = 0.0, transforms[2] = 0.0, transforms[3] = 1.0;
	transformsFull[0] = transforms[0];
	transformsFull[1] = transforms[1];
	transformsFull[2] = transforms[2];
	transformsFull[3] = transforms[3];
	transformsFull[4] = rotateQuat.x;
	transformsFull[5] = rotateQuat.y;
	transformsFull[6] = rotateQuat.z;
	transformsFull[7] = rotateQuat.w;
}

void Object::caculateLocalAABB(bool looseWidth, bool looseAll) {
	if (!mesh) return; // caculate AABB by yourself
	int vertexCount = mesh->vertexCount;
	if (vertexCount <= 0) return;
	vec4* vertices = mesh->vertices;
	vec4 first4 = localTransformMatrix * vertices[0];
	float sx = first4.x / first4.w;
	float sy = first4.y / first4.w;
	float sz = first4.z / first4.w;
	float lx = sx;
	float ly = sy;
	float lz = sz;
	for (int i = 1; i < vertexCount; i++) {
		vec4 vertex4 = vertices[i];
		vec4 local4 = localTransformMatrix * vertex4;
		vec3 local3(local4.x / local4.w, local4.y / local4.w, local4.z / local4.w);
		sx = sx > local3.x ? local3.x : sx;
		sy = sy > local3.y ? local3.y : sy;
		sz = sz > local3.z ? local3.z : sz;
		lx = lx < local3.x ? local3.x : lx;
		ly = ly < local3.y ? local3.y : ly;
		lz = lz < local3.z ? local3.z : lz;
	}
	vec3 minVertex(sx, sy, sz), maxVertex(lx, ly, lz);
	if (!bounding) bounding = new AABB(minVertex, maxVertex);
	else ((AABB*)bounding)->update(minVertex, maxVertex);

	if (looseWidth) {
		AABB* aabb = (AABB*)bounding;
		float maxWidth = aabb->sizex > aabb->sizez ? aabb->sizex : aabb->sizez;
		aabb->update(maxWidth, aabb->sizey, maxWidth);
	} 
	
	if (looseAll) {
		AABB* aabb = (AABB*)bounding;
		float maxSide = aabb->sizex;
		maxSide = maxSide < aabb->sizey ? aabb->sizey : maxSide;
		maxSide = maxSide < aabb->sizez ? aabb->sizez : maxSide;
		aabb->update(maxSide, maxSide, maxSide);
	}

	localBoundPosition.x=bounding->position.x;
	localBoundPosition.y=bounding->position.y;
	localBoundPosition.z=bounding->position.z;
}

void Object::updateLocalMatrices() {
	vertexTransform();
	normalTransform();
}

void Object::bindMaterial(int mid) {
	material = mid;
}

bool Object::checkInCamera(Camera* camera) {
	if (bounding)
		return bounding->checkWithCamera(camera->frustum, detailLevel);
	return true;
}

void Object::setBillboard(float sx, float sy, int mid) {
	if (billboard) delete billboard;
	billboard = new Billboard(sx, sy, mid);
}