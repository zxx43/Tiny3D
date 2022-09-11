/*
 * camera.h
 *
 *  Created on: 2017-4-1
 *      Author: a
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "frustum.h"

#define TRANS_TRANSLATE 1
#define TRANS_ROTATE_X 1<<1
#define TRANS_ROTATE_Y 1<<2

const vec3 ZERO_VEC3(0,0,0);
const vec3 UNIT_VEC3(1,1,1);
const vec4 UNIT_NEG_Z(0,0,-1,0);

class Camera {
private:
	vec4 rotXQuat, rotYQuat;
	float height;
	mat4 rotXMat, rotYMat, transMat, boundMat;
	vec4 lookDir4;
	bool needRefresh;
public:
	Frustum* frustum;
	vec3 position, lookDir, up;
	float fovy,aspect,zNear,zFar,invDist;
	float velocity;
	mat4 viewMatrix, projectMatrix, viewProjectMatrix;
	mat4 invViewProjectMatrix, invProjMatrix, invViewMatrix;
public:
	Camera(float height);
	Camera(const Camera& rhs);
	~Camera();
	void initPerspectCamera(float fovy,float aspect,float zNear,float zFar);
	void initOrthoCamera(float left,float right,float bottom,float top,float near,float far);
	void initOrthoCamera(float left, float right, float bottom, float top, float near, float far, float ex, float ey, float ez);
	void updateProjectMatrix(const mat4& matrix);
	void setView(const vec3& pos, const vec3& dir);
	void updateLook(const vec3& pos, const vec3& dir);
	void updateMoveable(uint transType);
	void forceRefresh() { needRefresh = true; }
	void updateFrustum(bool forceUpdate = false);

	void turnY(int ud);
	void turnX(int lr);
	void turnDX(float dx);
	void turnDY(float dy);
	void move(int dir,float speed);
	void moveTo(const vec3& pos);
	float getHeight();
	void copy(const Camera* src);
};


#endif /* CAMERA_H_ */
