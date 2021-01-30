#include "shadow.h"

Shadow::Shadow(Camera* view) {
	viewCamera=view;
	distance1 = 0.0;
	distance2 = 0.0;
	shadowMapSize = 0, shadowPixSize = 0, pixSize = 0;
	invViewMat.LoadIdentity();

	corners0 = new vec3[4];
	corners1 = new vec3[4];
	corners2 = new vec3[4];
	corners3 = new vec3[4];

	actLightCameraNear = new Camera(0); 
	actLightCameraMid = new Camera(0);
	actLightCameraFar = new Camera(0);
	renderLightCameraNear = new Camera(0);
	renderLightCameraMid = new Camera(0);
	renderLightCameraFar = new Camera(0);

	flushNear = false;
	flushMid = false;
	flushFar = false;
}

Shadow::~Shadow() {
	delete[] corners0; corners0 = NULL;
	delete[] corners1; corners1 = NULL;
	delete[] corners2; corners2 = NULL;
	delete[] corners3; corners3 = NULL;

	if (renderLightCameraNear != actLightCameraNear)
		delete renderLightCameraNear;
	renderLightCameraNear = NULL;
	if (renderLightCameraMid != actLightCameraMid)
		delete renderLightCameraMid;
	renderLightCameraMid = NULL;
	if (renderLightCameraFar != actLightCameraFar)
		delete renderLightCameraFar;
	renderLightCameraFar = NULL;

	delete actLightCameraNear; actLightCameraNear = NULL;
	delete actLightCameraMid; actLightCameraMid = NULL;
	delete actLightCameraFar; actLightCameraFar = NULL;
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

	corners0[0] = vec4(x0, y0, -nearDist, 1);
	corners0[1] = vec4(-x0, y0, -nearDist, 1);
	corners0[2] = vec4(x0, -y0, -nearDist, 1);
	corners0[3] = vec4(-x0, -y0, -nearDist, 1);

	corners1[0]=vec4(x1,y1,-level1,1);
	corners1[1]=vec4(-x1,y1,-level1,1);
	corners1[2]=vec4(x1,-y1,-level1,1);
	corners1[3]=vec4(-x1,-y1,-level1,1);

	corners2[0]=vec4(x2,y2,-level2,1);
	corners2[1]=vec4(-x2,y2,-level2,1);
	corners2[2]=vec4(x2,-y2,-level2,1);
	corners2[3]=vec4(-x2,-y2,-level2,1);

	corners3[0] = vec4(x3, y3, -farDist, 1);
	corners3[1] = vec4(-x3, y3, -farDist, 1);
	corners3[2] = vec4(x3, -y3, -farDist, 1);
	corners3[3] = vec4(-x3, -y3, -farDist, 1);

	gap = 50.0;
	inv2Gap = 1.0 / (gap * 2.0);

	center0 = vec4(0, 0, -(nearDist + (level1 + gap - nearDist) * 0.5), 1);
	center1 = vec4(0, 0, -(level1 + (level2 + gap - level1) * 0.5), 1);
	center2 = vec4(0, 0, -(level2 + (farDist - level2)*0.5), 1);

	vec3 c1 = vec3((level1 + gap) * tanHalfHFov, (level1 + gap) * tanHalfVFov, -level1 - gap);

	radius0 = (((vec3)center0) - c1).GetLength();
	radius1 = (((vec3)center1) - corners2[0]).GetLength();
	radius2 = (((vec3)center2) - corners3[0]).GetLength();

	radius = radius0;
	radius0 *= 1.2;

	actLightCameraNear->initOrthoCamera(-radius0, radius0, -radius0, radius0, -1.4 * radius0, 1.0 * radius0);
	actLightCameraMid->initOrthoCamera( -radius1, radius1, -radius1, radius1, -1.0 * radius1, 1.4 * radius1);
	actLightCameraFar->initOrthoCamera( -radius2, radius2, -radius2, radius2, -1.0 * radius2, 1.0 * radius2);
}

void Shadow::update(Camera* actCamera, const vec3& light) {
	lightDir = light;

	updateViewCamera(actCamera);
	updateLightCamera(actLightCameraNear, center0, radius0);
	updateLightCamera(actLightCameraMid, center1, radius1);
	//updateLightCamera(actLightCameraFar, center2, radius2);
}

void Shadow::updateViewCamera(Camera* actCamera) {
	invViewMat = mat4(actCamera->invViewMatrix);
	for (int i = 0; i < 3; ++i) {
		invViewMat.entries[i * 4 + 0] = (int)(invViewMat.entries[i * 4 + 0] * 0.1) * 10.0;
		invViewMat.entries[i * 4 + 1] = (int)(invViewMat.entries[i * 4 + 1] * 0.1) * 10.0;
		invViewMat.entries[i * 4 + 2] = (int)(invViewMat.entries[i * 4 + 2] * 0.1) * 10.0;
	}
	invViewMat.entries[12] = (int)(invViewMat.entries[12] * 0.005) * 200.0;
	invViewMat.entries[13] = (int)(invViewMat.entries[13] * 0.005) * 200.0;
	invViewMat.entries[14] = (int)(invViewMat.entries[14] * 0.005) * 200.0;
}

void Shadow::copyCameraData() {
	renderLightCameraNear->copy(actLightCameraNear);
	renderLightCameraMid->copy(actLightCameraMid);
	//renderLightCameraFar->copy(actLightCameraFar);
}

void Shadow::mergeCamera() {
	if (renderLightCameraNear && renderLightCameraNear != actLightCameraNear) {
		delete renderLightCameraNear;
		renderLightCameraNear = actLightCameraNear;
	}
	if (renderLightCameraMid && renderLightCameraMid != actLightCameraMid) {
		delete renderLightCameraMid;
		renderLightCameraMid = actLightCameraMid;
	}
	if (renderLightCameraFar && renderLightCameraFar != actLightCameraFar) {
		delete renderLightCameraFar;
		renderLightCameraFar = actLightCameraFar;
	}
}

void Shadow::updateLightCamera(Camera* lightCamera, const vec4& center, float radius) {
	lightCamera->updateLook((vec3)(invViewMat * center), lightDir);
}
