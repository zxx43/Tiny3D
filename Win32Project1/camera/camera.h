/*
 * camera.h
 *
 *  Created on: 2017-4-1
 *      Author: a
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "frustum.h"

#define TRANS_TRANSLATE 0
#define TRANS_ROTATE_X 1
#define TRANS_ROTATE_Y 2
#define TRANS_ROTATE_XY 3
#define TRANS_ALL 4

const VECTOR3D ZERO_VEC3(0,0,0);
const VECTOR3D UNIT_VEC3(1,1,1);
const VECTOR4D UNIT_NEG_Z(0,0,-1,0);

class Camera {
private:
	float xrot,yrot,height;
	MATRIX4X4 rotXMat, rotYMat, transMat;
	VECTOR4D lookDir4;
public:
	bool simpleCheck, isMain;
	Frustum* frustum;
	VECTOR3D position, lookDir, up;
	float fovy,aspect,zNear,zFar;
	MATRIX4X4 viewMatrix, projectMatrix, viewProjectMatrix;
	MATRIX4X4 invViewProjectMatrix;

	Camera(float height);
	~Camera();
	void initPerspectCamera(float fovy,float aspect,float zNear,float zFar);
	void initOrthoCamera(float left,float right,float bottom,float top,float near,float far);
	void setView(const VECTOR3D& pos, const VECTOR3D& dir);
	void updateLook(const VECTOR3D& pos, const VECTOR3D& dir);
	void updateMoveable(uint transType);
	void updateFrustum();

	void turnY(int ud);
	void turnX(int lr);
	void turnDX(float dx);
	void turnDY(float dy);
	void move(int dir,float speed);
	void moveTo(const VECTOR3D& pos);
	float getHeight();
	void copy(Camera* src);
};


#endif /* CAMERA_H_ */
