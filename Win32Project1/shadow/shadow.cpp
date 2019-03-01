#include "shadow.h"

Shadow::Shadow(Camera* view) {
	viewCamera=view;
	distance1 = 0.0;
	distance2 = 0.0;
	shadowMapSize = 0, shadowPixSize = 0;

	corners0 = new VECTOR3D[4];
	corners1 = new VECTOR3D[4];
	corners2 = new VECTOR3D[4];
	corners3 = new VECTOR3D[4];

	lightCameraNear = new Camera(0); 
	lightCameraMid = new Camera(0);
	lightCameraFar = new Camera(0);
}

Shadow::~Shadow() {
	delete[] corners0; corners0=NULL;
	delete[] corners1; corners1=NULL;
	delete[] corners2; corners2=NULL;
	delete[] corners3; corners3=NULL;
	delete lightCameraNear; lightCameraNear=NULL;
	delete lightCameraMid; lightCameraMid=NULL;
	delete lightCameraFar; lightCameraFar=NULL;
}

void Shadow::prepareViewCamera(float dist1, float dist2) {
	distance1 = dist1;
	distance2 = dist2;

	nearDist=viewCamera->zNear;
	level1=distance1+nearDist;
	level2=distance2+nearDist;
	farDist=viewCamera->zFar;

	float fovy=viewCamera->fovy;
	float aspect=viewCamera->aspect;
	float tanHalfHFov=aspect*tanf(fovy*0.5*A2R);
	float tanHalfVFov=tanf(fovy*0.5*A2R);

	float x0=nearDist*tanHalfHFov;
	float x1=level1*tanHalfHFov;
	float x2=level2*tanHalfHFov;
	float x3=farDist*tanHalfHFov;
	float y0=nearDist*tanHalfVFov;
	float y1=level1*tanHalfVFov;
	float y2=level2*tanHalfVFov;
	float y3=farDist*tanHalfVFov;

	corners0[0] = VECTOR4D(x0, y0, -nearDist, 1);
	corners0[1] = VECTOR4D(-x0, y0, -nearDist, 1);
	corners0[2] = VECTOR4D(x0, -y0, -nearDist, 1);
	corners0[3] = VECTOR4D(-x0, -y0, -nearDist, 1);

	corners1[0]=VECTOR4D(x1,y1,-level1,1);
	corners1[1]=VECTOR4D(-x1,y1,-level1,1);
	corners1[2]=VECTOR4D(x1,-y1,-level1,1);
	corners1[3]=VECTOR4D(-x1,-y1,-level1,1);

	corners2[0]=VECTOR4D(x2,y2,-level2,1);
	corners2[1]=VECTOR4D(-x2,y2,-level2,1);
	corners2[2]=VECTOR4D(x2,-y2,-level2,1);
	corners2[3]=VECTOR4D(-x2,-y2,-level2,1);

	corners3[0] = VECTOR4D(x3, y3, -farDist, 1);
	corners3[1] = VECTOR4D(-x3, y3, -farDist, 1);
	corners3[2] = VECTOR4D(x3, -y3, -farDist, 1);
	corners3[3] = VECTOR4D(-x3, -y3, -farDist, 1);

	center0 = VECTOR4D(0, 0, -(nearDist+(level1-nearDist)*0.5), 1);
	center1 = VECTOR4D(0,0,-(level1+(level2-level1)*0.5),1);
	center2 = VECTOR4D(0, 0, -(level2 + (farDist - level2)*0.5), 1);

	radius0 = (((VECTOR3D)center0) - corners1[0]).GetLength();
	radius1=(((VECTOR3D)center1) - corners2[0]).GetLength();
	radius2=(((VECTOR3D)center2) - corners3[0]).GetLength();

	lightCameraNear->initOrthoCamera(-radius0, radius0, -radius0, radius0, -1.0001 * radius0, 1.0001 * radius0);
	lightCameraMid->initOrthoCamera(-radius1, radius1, -radius1, radius1, -1.0001 * radius1, 1.0001 * radius1);
	lightCameraFar->initOrthoCamera(-radius2, radius2, -radius2, radius2, -1.0001 * radius2, 1.0001 * radius2);
}

void Shadow::update(const VECTOR3D& light) {
	lightDir = light;

	updateLightCamera(lightCameraNear, &center0, radius0);
	updateLightCamera(lightCameraMid, &center1, radius1);
	updateLightCamera(lightCameraFar, &center2, radius2);

	lightNearMat = lightCameraNear->viewProjectMatrix;
	lightMidMat = lightCameraMid->viewProjectMatrix;
	lightFarMat = lightCameraFar->viewProjectMatrix;
}

void Shadow::updateLightCamera(Camera* lightCamera, const VECTOR4D* center, float radius) {
	lightCamera->updateLook((VECTOR3D)(viewCamera->invViewMatrix * (*center)), lightDir);
}
