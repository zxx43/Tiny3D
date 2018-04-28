#include "staticObject.h"
#include "../util/util.h"

StaticObject::StaticObject(Mesh* mesh) :Object() {
	this->mesh = mesh;
	anglex = 0; angley = 0; anglez = 0;
}

StaticObject::StaticObject(Mesh* mesh, Mesh* meshMid, Mesh* meshLow) : Object() {
	this->mesh = mesh;
	this->meshMid = meshMid;
	this->meshLow = meshLow;
	anglex = 0; angley = 0; anglez = 0;
}

StaticObject::StaticObject(const StaticObject& rhs) {
	mesh = rhs.mesh;
	meshMid = rhs.meshMid;
	meshLow = rhs.meshLow;
	material = rhs.material;
	if (rhs.bounding)
		bounding = rhs.bounding->clone();
	else
		bounding = NULL;
	position = rhs.position;
	anglex = rhs.anglex; angley = rhs.angley; anglez = rhs.anglez;
	sizex = rhs.sizex; sizey = rhs.sizey; sizez = rhs.sizez;
}

StaticObject::~StaticObject() {}

StaticObject* StaticObject::clone() {
	return new StaticObject(*this);
}

void StaticObject::vertexTransform() {
	MATRIX4X4 translateMat=translate(position.x,position.y,position.z);
	MATRIX4X4 rotateMat=rotateZ(anglez)*rotateY(angley)*rotateX(anglex);
	MATRIX4X4 scaleMat=scaleZ(sizez)*scaleY(sizey)*scaleX(sizex);
	localTransformMatrix=translateMat*rotateMat*scaleMat;
}

void StaticObject::normalTransform() {
	if(sizex==sizey&&sizey==sizez) {
		normalMatrix=localTransformMatrix;
		return;
	}
	normalMatrix=localTransformMatrix.GetInverse().GetTranspose();
}

void StaticObject::setPosition(float x, float y, float z) {
	position.x = x;
	position.y = y;
	position.z = z;
	updateLocalMatrices();
}

void StaticObject::setRotation(float ax, float ay, float az) {
	anglex = ax; angley = ay; anglez = az;
	updateLocalMatrices();
}

void StaticObject::setSize(float sx, float sy, float sz) {
	sizex = sx; sizey = sy; sizez = sz;
	updateLocalMatrices();
}





