#include "object.h"
#include <stdlib.h>
#include "../constants/constants.h"

Object::Object() {
	position = VECTOR3D(0, 0, 0);
	sizex = 1.0; sizey = 1.0; sizez = 1.0;
	localTransformMatrix.LoadIdentity();
	normalMatrix.LoadIdentity();
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
}

Object::Object(const Object& rhs) {

}

Object::~Object() {
	if (bounding) delete bounding;
	bounding = NULL;
	if (billboard) delete billboard;
	billboard = NULL;
}

void Object::caculateLocalAABB(bool looseWidth, bool looseAll) {
	if (!mesh) return; // caculate AABB by yourself
	int vertexCount = mesh->vertexCount;
	if (vertexCount <= 0) return;
	VECTOR4D* vertices = mesh->vertices;
	VECTOR4D first4 = localTransformMatrix * vertices[0];
	float sx = first4.x / first4.w;
	float sy = first4.y / first4.w;
	float sz = first4.z / first4.w;
	float lx = sx;
	float ly = sy;
	float lz = sz;
	for (int i = 1; i < vertexCount; i++) {
		VECTOR4D vertex4 = vertices[i];
		VECTOR4D local4 = localTransformMatrix * vertex4;
		VECTOR3D local3(local4.x / local4.w, local4.y / local4.w, local4.z / local4.w);
		sx = sx > local3.x ? local3.x : sx;
		sy = sy > local3.y ? local3.y : sy;
		sz = sz > local3.z ? local3.z : sz;
		lx = lx < local3.x ? local3.x : lx;
		ly = ly < local3.y ? local3.y : ly;
		lz = lz < local3.z ? local3.z : lz;
	}
	VECTOR3D minVertex(sx, sy, sz), maxVertex(lx, ly, lz);
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