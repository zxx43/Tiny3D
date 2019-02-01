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
	float nearDist, farDist;
	VECTOR3D* corners0;
	VECTOR3D* corners1;
	VECTOR3D* corners2;
	VECTOR3D* corners3;
	VECTOR4D center0,center1,center2;
	float radius0,radius1,radius2;

	void updateLightCamera(Camera* lightCamera,const VECTOR4D* center,float radius);
public:
	float distance1, distance2;
	Camera* lightCameraNear;
	Camera* lightCameraMid;
	Camera* lightCameraFar;
	MATRIX4X4 lightNearMat, lightMidMat, lightFarMat;
	float level1,level2;
	float shadowMapSize,shadowPixSize;
	VECTOR3D lightDir;

	Shadow(Camera* view,float distance1,float distance2);
	~Shadow();

	void prepareViewCamera();
	void update(const VECTOR3D& light);
};


#endif /* SHADOW_H_ */
