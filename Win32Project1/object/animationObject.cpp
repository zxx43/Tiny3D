#include "animationObject.h"
#include "../util/util.h"

AnimationObject::AnimationObject(Animation* anim):Object() {
	animation=anim; // no mesh!
	anglex=0; angley=0; anglez=0;
}

AnimationObject::AnimationObject(const AnimationObject& rhs) {
	animation=rhs.animation;
	if(rhs.bounding)
		bounding=rhs.bounding->clone();
	else
		bounding=NULL;
	anglex=rhs.anglex; angley=rhs.angley; anglez=rhs.anglez;

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
}

AnimationObject::~AnimationObject() {}

AnimationObject* AnimationObject::clone() {
	return new AnimationObject(*this);
}

void AnimationObject::vertexTransform() {
	translateMat = translate(position.x, position.y, position.z);
	rotateMat = rotateZ(anglez)*rotateY(angley)*rotateX(anglex);
	scaleMat = scale(size.x, size.y, size.z);
	localTransformMatrix = translateMat * rotateMat*scaleMat;
}

void AnimationObject::normalTransform() {
	if (size.x == size.y && size.y == size.z) {
		normalMatrix = localTransformMatrix;
		return;
	}
	normalMatrix = localTransformMatrix.GetInverse().GetTranspose();
}

void AnimationObject::setPosition(float x,float y,float z) {
	position.x=x;
	position.y=y;
	position.z=z;
	updateLocalMatrices();
}

void AnimationObject::setRotation(float ax,float ay,float az) {
	anglex=ax; angley=ay; anglez=az;
	updateLocalMatrices();
}

void AnimationObject::setSize(float sx,float sy,float sz) {
	size = vec3(sx, sy, sz);
	updateLocalMatrices();
	if (billboard) {
		billboard->data[0] *= size.x;
		billboard->data[1] *= size.y;
	}
}
