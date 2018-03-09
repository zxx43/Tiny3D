/*
 * shadow.h
 *
 *  Created on: 2017-8-5
 *      Author: a
 */

#ifndef SHADOW_H_
#define SHADOW_H_

#include "../camera/camera.h"

class Shadow {
private:
	Camera* viewCamera;
	MATRIX4X4 invViewMatrix;
	float distance1,distance2;
	float nearDist, farDist;
	VECTOR4D* corners0;
	VECTOR4D* corners1;
	VECTOR4D* corners2;
	VECTOR4D* corners3;
	VECTOR4D center0,center1,center2;
	float radius0,radius1,radius2;

	void updateLightCamera(Camera* lightCamera,VECTOR4D center,float radius);
public:
	Camera* lightCameraNear;
	Camera* lightCameraMid;
	Camera* lightCameraFar;
	MATRIX4X4 lightNearMat, lightMidMat, lightFarMat;
	float level1,level2;
	VECTOR3D lightDir;

	Shadow(Camera* view,float distance1,float distance2);
	~Shadow();

	void prepareViewCamera();
	void update(const VECTOR3D& light);
};


#endif /* SHADOW_H_ */
