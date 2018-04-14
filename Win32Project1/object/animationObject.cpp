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
	position.x=rhs.position.x; position.y=rhs.position.y; position.z=rhs.position.z;
	anglex=rhs.anglex; angley=rhs.angley; anglez=rhs.anglez;
	sizex=rhs.sizex; sizey=rhs.sizey; sizez=rhs.sizez;
}

AnimationObject::~AnimationObject() {}

AnimationObject* AnimationObject::clone() {
	return new AnimationObject(*this);
}

void AnimationObject::vertexTransform() {
	MATRIX4X4 translateMat=translate(position.x,position.y,position.z);
	MATRIX4X4 rotateMat=rotateZ(anglez)*rotateY(angley)*rotateX(anglex);
	MATRIX4X4 scaleMat=scaleZ(sizez)*scaleY(sizey)*scaleX(sizex);
	localTransformMatrix=translateMat*rotateMat*scaleMat;
}

void AnimationObject::normalTransform() {
	if(sizex==sizey&&sizey==sizez) {
		normalMatrix=localTransformMatrix;
		return;
	}
	normalMatrix=localTransformMatrix.GetInverse().GetTranspose();
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
	sizex=sx; sizey=sy; sizez=sz;
	updateLocalMatrices();
}
