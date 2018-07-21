#include "camera.h"
#include "../util/util.h"

Camera::Camera(float height) {
	position.x=0; position.y=0; position.z=0;
	lookDir = VECTOR3D();
	lookDir4 = VECTOR4D();
	up.x=0; up.y=1; up.z=0;

	xrot=0; yrot=0;
	this->height = height;

	frustum=new Frustum();

	simpleCheck = false;
	isMain = false;
}

Camera::~Camera() {
	delete frustum;
	frustum=NULL;
}

void Camera::initPerspectCamera(float fovy,float aspect,float zNear,float zFar) {
	this->fovy=fovy;
	this->aspect=aspect;
	this->zNear=zNear;
	this->zFar=zFar;
	projectMatrix=perspective(fovy,aspect,zNear,zFar);
}

void Camera::initOrthoCamera(float left,float right,float bottom,float top,float near,float far) {
	projectMatrix=ortho(left,right,bottom,top,near,far);
}

void Camera::setView(const VECTOR3D& pos, const VECTOR3D& dir) {
	position.x=pos.x; position.y=pos.y; position.z=pos.z;
	lookDir.x=dir.x; lookDir.y=dir.y; lookDir.z=dir.z;

	VECTOR3D center(lookDir.x+position.x,lookDir.y+position.y,lookDir.z+position.z);
	viewMatrix=lookAt(position.x,position.y,position.z,center.x,center.y,center.z,up.x,up.y,up.z);
}

void Camera::updateLook(const VECTOR3D& pos, const VECTOR3D& dir) {
	setView(pos,dir);
	updateFrustum();
}

void Camera::updateMoveable(uint transType) {
	if (transType == TRANS_TRANSLATE)
		transMat = translate(-position.x, -position.y, -position.z);
	else if (transType == TRANS_ROTATE_Y) 
		rotXMat = rotateX(-yrot);
	else if (transType == TRANS_ROTATE_X)
		rotYMat = rotateY(-xrot);
	else if (transType == TRANS_ROTATE_XY) {
		rotXMat = rotateX(-yrot);
		rotYMat = rotateY(-xrot);
	} else if (transType == TRANS_ALL) {
		transMat = translate(-position.x, -position.y, -position.z);
		rotXMat = rotateX(-yrot);
		rotYMat = rotateY(-xrot);
	}
	viewMatrix = rotXMat * rotYMat * transMat;

	lookDir4 = viewMatrix.GetInverse() * UNIT_NEG_Z;
	lookDir.x = lookDir4.x;
	lookDir.y = lookDir4.y;
	lookDir.z = lookDir4.z;
}

void Camera::updateFrustum() {
	viewProjectMatrix = projectMatrix * viewMatrix;
	invViewProjectMatrix = viewProjectMatrix.GetInverse();
	lookDir.Normalize();
	frustum->update(invViewProjectMatrix, lookDir);
}

void Camera::turnX(int lr) {
	switch(lr) {
		case LEFT:
			turnDX(D_ROTATION);
			break;
		case RIGHT:
			turnDX(-D_ROTATION);
			break;
	}
	updateMoveable(TRANS_ROTATE_X);
}

void Camera::turnY(int ud) {
	switch(ud) {
		case UP:
			turnDY(D_ROTATION);
			break;
		case DOWN:
			turnDY(-D_ROTATION);
			break;
	}
	updateMoveable(TRANS_ROTATE_Y);
}

void Camera::turnDX(float dx) {
	xrot += dx;
	RestrictAngle(xrot);
}

void Camera::turnDY(float dy) {
	yrot += dy;
	RestrictAngle(yrot);
}

void Camera::move(int dir,float speed) {
	float xz=angleToRadian(xrot);
	float yz=angleToRadian(yrot);
	float cosYZ = speed * cosf(yz);
	float dx = cosYZ * sinf(xz);
	float dy = speed * sinf(yz);
	float dz = cosYZ * cosf(xz);

	switch(dir) {
		case DOWN:
			if (height - speed >  2.0) {
				height -= speed;
				position.y -= speed;
			}
			break;
		case UP:
			height += speed;
			position.y += speed;
			break;
		case RIGHT:
			position.x+=speed*cosf(xz);
			position.z-=speed*sinf(xz);
			break;
		case LEFT:
			position.x-=speed*cosf(xz);
			position.z+=speed*sinf(xz);
			break;
		case MFAR:
			position.x+=dx;
			position.y-=dy;
			position.z+=dz;
			break;
		case MNEAR:
			position.x-=dx;
			position.y+=dy;
			position.z-=dz;
			break;
	}
	updateMoveable(TRANS_TRANSLATE);
}

void Camera::moveTo(const VECTOR3D& pos) {
	position.x = pos.x; 
	position.y = pos.y; 
	position.z = pos.z;
	updateMoveable(TRANS_TRANSLATE);
}

float Camera::getHeight() {
	return height;
}

void Camera::copy(Camera* src) {
	viewMatrix = src->viewMatrix;
	projectMatrix = src->projectMatrix;
	viewProjectMatrix = src->viewProjectMatrix;
	invViewProjectMatrix = src->invViewProjectMatrix;
	position = src->position;
}
