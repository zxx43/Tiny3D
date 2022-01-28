#include "camera.h"
#include "../util/util.h"

Camera::Camera(float height) {
	position = vec3(0.0, 0.0, 0.0);
	lookDir = vec3();
	lookDir4 = vec4();
	up = vec3(0.0, 1.0, 0.0);

	xrot = 0; yrot = 0;
	this->height = height;
	velocity = 1.0;

	frustum = new Frustum();
	needRefresh = true;
}

Camera::~Camera() {
	delete frustum; frustum = NULL;
}

void Camera::initPerspectCamera(float fovy, float aspect, float zNear, float zFar) {
	this->fovy = fovy;
	this->aspect = aspect;
	this->zNear = zNear;
	this->zFar = zFar;
	invDist = 1.0 / (this->zFar - this->zNear);
	projectMatrix = perspective(fovy, aspect, zNear, zFar);
	invProjMatrix = projectMatrix.GetInverse();
	boundMat = projectMatrix;
	needRefresh = true;
}

void Camera::initOrthoCamera(float left, float right, float bottom, float top, float near, float far) {
	zNear = near;
	zFar = far;
	invDist = 1.0 / (zFar - zNear);
	projectMatrix = ortho(left, right, bottom, top, near, far);
	invProjMatrix = projectMatrix.GetInverse();
	boundMat = projectMatrix;
	needRefresh = true;
}

void Camera::initOrthoCamera(float left, float right, float bottom, float top, float near, float far, float ex, float ey, float ez) {
	zNear = near;
	zFar = far;
	invDist = 1.0 / (zFar - zNear);
	projectMatrix = ortho(left, right, bottom, top, near, far);
	invProjMatrix = projectMatrix.GetInverse();
	boundMat = ortho(left * ex, right * ex, bottom * ey, top * ey, near * ez, far * ez);
	needRefresh = true;
}

void Camera::updateProjectMatrix(const mat4& matrix) {
	projectMatrix = matrix;
	invProjMatrix = projectMatrix.GetInverse();
	needRefresh = true;
	updateFrustum();
}

void Camera::setView(const vec3& pos, const vec3& dir) {
	position = pos;
	lookDir = dir;
	vec3 center = lookDir + position;
	viewMatrix = lookAt(position, center, up);
	invViewMatrix = viewMatrix.GetInverse();
	needRefresh = true;
}

void Camera::updateLook(const vec3& pos, const vec3& dir) {
	setView(pos,dir);
	//updateFrustum();
}

void Camera::updateMoveable(uint transType) {
	if (transType & TRANS_TRANSLATE)
		transMat = translate(-position);
	if (transType & TRANS_ROTATE_Y) 
		rotXMat = rotateX(-yrot);
	if (transType & TRANS_ROTATE_X)
		rotYMat = rotateY(-xrot);

	viewMatrix = rotXMat * rotYMat * transMat;
	invViewMatrix = viewMatrix.GetInverse();

	lookDir4 = invViewMatrix * UNIT_NEG_Z;
	lookDir.x = lookDir4.x;
	lookDir.y = lookDir4.y;
	lookDir.z = lookDir4.z;

	needRefresh = true;
}

void Camera::updateFrustum() {
	if (needRefresh) {
		viewProjectMatrix = projectMatrix * viewMatrix;
		invViewProjectMatrix = viewProjectMatrix.GetInverse();
		lookDir.Normalize();
		frustum->update((boundMat * viewMatrix).GetInverse(), lookDir);
		needRefresh = false;
	}
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
			if (height - speed >  MIN_HEIGHT) {
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

void Camera::moveTo(const vec3& pos) {
	position = pos;
	updateMoveable(TRANS_TRANSLATE);
}

float Camera::getHeight() {
	return height;
}

void Camera::copy(Camera* src) {
	xrot = src->xrot;
	yrot = src->yrot;
	height = src->height;
	rotXMat = src->rotXMat;
	rotYMat = src->rotYMat;
	transMat = src->transMat;
	lookDir4 = src->lookDir4;

	viewMatrix = src->viewMatrix;
	projectMatrix = src->projectMatrix;
	boundMat = src->boundMat;
	viewProjectMatrix = src->viewProjectMatrix;
	invViewProjectMatrix = src->invViewProjectMatrix;
	invProjMatrix = src->invProjMatrix;
	invViewMatrix = src->invViewMatrix;
	position = src->position;
	lookDir = src->lookDir; 
	up = src->up;
	fovy = src->fovy;
	aspect = src->aspect;
	zNear = src->zNear; 
	zFar = src->zFar;
	invDist = src->invDist;
	velocity = src->velocity;

	needRefresh = src->needRefresh;
}
