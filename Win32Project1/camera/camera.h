/*
 * camera.h
 *
 *  Created on: 2017-4-1
 *      Author: a
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "frustum.h"

const VECTOR3D ZERO_VEC3(0,0,0);
const VECTOR3D UNIT_VEC3(1,1,1);
const VECTOR4D UNIT_NEG_Z(0,0,-1,0);

class Camera {
private:
	float xrot,yrot,height;
public:
	Frustum* frustum;
	VECTOR3D position, lookDir, up;
	float fovy,aspect,zNear,zFar;
	MATRIX4X4 viewMatrix, projectMatrix;

	Camera(float height);
	~Camera();
	void initPerspectCamera(float fovy,float aspect,float zNear,float zFar);
	void initOrthoCamera(float left,float right,float bottom,float top,float near,float far);
	void setView(const VECTOR3D& pos, const VECTOR3D& dir);
	void updateLook(const VECTOR3D& pos, const VECTOR3D& dir);
	void updateMoveable();
	void updateFrustum();

	void turnY(int ud);
	void turnX(int lr);
	void move(int dist,float speed);
	void moveTo(const VECTOR3D& pos);
	float getHeight();
	void copy(Camera* src);
};


#endif /* CAMERA_H_ */
