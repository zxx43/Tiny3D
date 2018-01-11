#include "billboardObject.h"
#include "../util/util.h"

BillboardObject::BillboardObject(Mesh* mesh):Object() {
	this->mesh=mesh;
	localTransformMatrix.LoadIdentity();
	normalMatrix.LoadIdentity();
}

BillboardObject::BillboardObject(const BillboardObject& rhs) {
	mesh=rhs.mesh;
	material = rhs.material;
	if(rhs.bounding)
		bounding=rhs.bounding->clone();
	else
		bounding=NULL;
	position=rhs.position;
	sizex=rhs.sizex; sizey=rhs.sizey; sizez=rhs.sizez;
	localTransformMatrix=rhs.localTransformMatrix;
	normalMatrix.LoadIdentity();
}

BillboardObject::~BillboardObject() {}

BillboardObject* BillboardObject::clone() {
	return new BillboardObject(*this);
}

void BillboardObject::vertexTransform() {
	MATRIX4X4 translateMat=translate(position.x,position.y,position.z);
	MATRIX4X4 scaleMat=scaleZ(sizez)*scaleY(sizey)*scaleX(sizex);
	localTransformMatrix=translateMat*scaleMat;
}

void BillboardObject::normalTransform() {}

void BillboardObject::setPosition(float x, float y, float z) {
	position.x = x;
	position.y = y;
	position.z = z;
	updateLocalMatrices();
}

void BillboardObject::setSize(float sx, float sy, float sz) {
	sizex = sx; sizey = sy; sizez = sz;
	updateLocalMatrices();
}

void BillboardObject::caculateLocalAABB(bool looseWidth, bool looseAll) {
	Object::caculateLocalAABB(true, looseAll); // Always looseWidth
}