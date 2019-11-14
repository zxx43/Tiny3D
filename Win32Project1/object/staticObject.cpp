#include "staticObject.h"
#include "../util/util.h"

StaticObject::StaticObject(Mesh* mesh) :Object() {
	this->mesh = mesh;
	this->meshMid = mesh;
	this->meshLow = mesh;
	anglex = 0; angley = 0; anglez = 0;

	transforms = (float*)malloc(4 * sizeof(float));
	transformsFull = (buff*)malloc(12 * sizeof(buff));
}

StaticObject::StaticObject(Mesh* mesh, Mesh* meshMid, Mesh* meshLow) :Object() {
	this->mesh = mesh;
	this->meshMid = meshMid;
	this->meshLow = meshLow;
	anglex = 0; angley = 0; anglez = 0;

	transforms = (float*)malloc(4 * sizeof(float));
	transformsFull = (buff*)malloc(12 * sizeof(buff));
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
	anglex = rhs.anglex; angley = rhs.angley; anglez = rhs.anglez;

	position = rhs.position;
	size = rhs.size;
	localTransformMatrix = rhs.localTransformMatrix;
	normalMatrix = rhs.normalMatrix;
	localBoundPosition = rhs.localBoundPosition;
	rotateQuat = rhs.rotateQuat;
	boundInfo = rhs.boundInfo;

	rotateMat = rhs.rotateMat;
	translateMat = rhs.translateMat;
	scaleMat = rhs.scaleMat;

	genShadow = rhs.genShadow;
	detailLevel = rhs.detailLevel;

	if (rhs.billboard)
		setBillboard(rhs.billboard->data[0], rhs.billboard->data[1], rhs.billboard->material);
	if (rhs.transforms) {
		transforms = (float*)malloc(4 * sizeof(float));
		memcpy(transforms, rhs.transforms, 4 * sizeof(float));
	}
	if (rhs.transformsFull) {
		transformsFull = (buff*)malloc(12 * sizeof(buff));
		memcpy(transformsFull, rhs.transformsFull, 12 * sizeof(buff));
	}
}

StaticObject::~StaticObject() {
	if (transforms) free(transforms);
	transforms = NULL;
	if (transformsFull) free(transformsFull);
	transformsFull = NULL;
}

StaticObject* StaticObject::clone() {
	return new StaticObject(*this);
}

void StaticObject::vertexTransform() {
	translateMat = translate(position.x, position.y, position.z);
	rotateMat = rotateZ(anglez)*rotateY(angley)*rotateX(anglex);
	scaleMat = scale(size.x, size.y, size.z);
	localTransformMatrix = translateMat * rotateMat*scaleMat;
}

void StaticObject::normalTransform() {
	if (size.x == size.y && size.y == size.z) {
		normalMatrix = localTransformMatrix;
		return;
	}
	normalMatrix = localTransformMatrix.GetInverse().GetTranspose();
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
	size = vec3(sx, sy, sz);
	updateLocalMatrices();
	if (billboard) {
		billboard->data[0] *= size.x;
		billboard->data[1] *= size.y;
	}
}





