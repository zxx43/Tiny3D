#include "shadow.h"

Shadow::Shadow(Camera* view, float sn, float sm, float sf) {
	viewCamera=view;
	distance1 = 0.0;
	distance2 = 0.0;
	shadowMapSize = 0, shadowPixSize = 0, pixSize = 0;
	invViewMat.LoadIdentity();

	corners0 = new vec3[4];
	corners1 = new vec3[4];
	corners2 = new vec3[4];
	corners3 = new vec3[4];

	actLightCameraDyn = new Camera(0);
	actLightCameraNear = new Camera(0); 
	actLightCameraMid = new Camera(0);
	actLightCameraFar = new Camera(0);
	renderLightCameraDyn = new Camera(0);
	renderLightCameraNear = new Camera(0);
	renderLightCameraMid = new Camera(0);
	renderLightCameraFar = new Camera(0);

	flushDyn = false;
	flushNear = false;
	flushMid = false;
	flushFar = false;

	sizeNear = sn, sizeMid = sm, sizeFar = sf;
}

Shadow::~Shadow() {
	delete[] corners0; corners0 = NULL;
	delete[] corners1; corners1 = NULL;
	delete[] corners2; corners2 = NULL;
	delete[] corners3; corners3 = NULL;

	if (renderLightCameraDyn != actLightCameraDyn)
		delete renderLightCameraDyn;
	renderLightCameraDyn = NULL;
	if (renderLightCameraNear != actLightCameraNear)
		delete renderLightCameraNear;
	renderLightCameraNear = NULL;
	if (renderLightCameraMid != actLightCameraMid)
		delete renderLightCameraMid;
	renderLightCameraMid = NULL;
	if (renderLightCameraFar != actLightCameraFar)
		delete renderLightCameraFar;
	renderLightCameraFar = NULL;

	delete actLightCameraDyn; actLightCameraDyn = NULL;
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

	corners0[0] = vec3(x0, y0, -nearDist);
	corners0[1] = vec3(-x0, y0, -nearDist);
	corners0[2] = vec3(x0, -y0, -nearDist);
	corners0[3] = vec3(-x0, -y0, -nearDist);

	corners1[0] = vec3(x1, y1, -level1);
	corners1[1] = vec3(-x1, y1, -level1);
	corners1[2] = vec3(x1, -y1, -level1);
	corners1[3] = vec3(-x1, -y1, -level1);

	corners2[0] = vec3(x2, y2, -level2);
	corners2[1] = vec3(-x2, y2, -level2);
	corners2[2] = vec3(x2, -y2, -level2);
	corners2[3] = vec3(-x2, -y2, -level2);

	corners3[0] = vec3(x3, y3, -farDist);
	corners3[1] = vec3(-x3, y3, -farDist);
	corners3[2] = vec3(x3, -y3, -farDist);
	corners3[3] = vec3(-x3, -y3, -farDist);

	gap = 50.0f;
	inv2Gap = 1.0 / (gap * 2.0);

	center0 = vec4(0, 0, -(nearDist + (level1 + gap - nearDist) * 0.5), 1);
	center1 = vec4(0, 0, -(level1 + (level2 + gap - level1) * 0.5), 1);
	center2 = vec4(0, 0, -(level2 + (farDist - level2)*0.5), 1);

	vec3 c1 = vec3((level1 + gap) * tanHalfHFov, (level1 + gap) * tanHalfVFov, -level1 - gap);

	radius0 = (((vec3)center0) - c1).GetLength();
	radius1 = (((vec3)center1) - corners2[0]).GetLength();
	radius2 = (((vec3)center2) - corners3[0]).GetLength();
	radius = radius0;

	actLightCameraDyn->initOrthoCamera(-radius0, radius0, -radius0, radius0, -1.3 * radius0, 1.3 * radius0, 1.5, 1.5, 1.5);
	actLightCameraNear->initOrthoCamera(-radius0, radius0, -radius0, radius0, -1.3 * radius0, 1.3 * radius0, 1.5, 1.5, 1.5);
	actLightCameraMid->initOrthoCamera( -radius1, radius1, -radius1, radius1, -1.2 * radius1, 1.2 * radius1, 1.5, 1.5, 1.5);
	actLightCameraFar->initOrthoCamera( -radius2, radius2, -radius2, radius2, -1.0 * radius2, 1.0 * radius2);

	shadowProjDyn = actLightCameraDyn->projectMatrix;
	shadowProjNear = actLightCameraNear->projectMatrix;
	shadowProjMid = actLightCameraMid->projectMatrix;
	shadowProjFar = actLightCameraFar->projectMatrix;
}

void Shadow::update(Camera* actCamera, const vec3& light) {
	lightDir = light;

	updateViewCamera(actCamera);
	updateLightCamera(actLightCameraDyn, center0, radius0);
	updateLightCamera(actLightCameraNear, center0, radius0);
	updateLightCamera(actLightCameraMid, center1, radius1);
	updateLightCamera(actLightCameraFar, center2, radius2);

	actLightCameraDyn->updateProjectMatrix(genSnap(shadowProjDyn, actLightCameraDyn, sizeNear));
	actLightCameraNear->updateProjectMatrix(genSnap(shadowProjNear, actLightCameraNear, sizeNear));
	actLightCameraMid->updateProjectMatrix(genSnap(shadowProjMid, actLightCameraMid, sizeMid));
	actLightCameraFar->updateProjectMatrix(genSnap(shadowProjFar, actLightCameraFar, sizeFar));
}

void Shadow::updateViewCamera(Camera* actCamera) {
	invViewMat = mat4(actCamera->invViewMatrix);
	for (int i = 0; i < 3; ++i) {
		invViewMat.entries[i * 4 + 0] = roundf(invViewMat.entries[i * 4 + 0] * 1.0f) * 1.0f;
		invViewMat.entries[i * 4 + 1] = roundf(invViewMat.entries[i * 4 + 1] * 1.0f) * 1.0f;
		invViewMat.entries[i * 4 + 2] = roundf(invViewMat.entries[i * 4 + 2] * 1.0f) * 1.0f;
	}
}

void Shadow::updateLightCamera(Camera* lightCamera, const vec4& center, float radius) {
	lightCamera->updateLook((vec3)(invViewMat * center), lightDir);
}

mat4 Shadow::genSnap(const mat4& projInit, Camera* lightCamera, float size) {
	vec4 shadowOrigin = projInit * lightCamera->viewMatrix * vec4(0.0f, 0.0f, 0.0f, 1.0f);
	shadowOrigin /= shadowOrigin.w;
	shadowOrigin = (shadowOrigin + 1.0) * 0.5f * size; // NDC to [0,1] and than mul shadowmap size
	vec3 roundedOrigin = vec3((int)(shadowOrigin.x), (int)(shadowOrigin.y), (int)(shadowOrigin.z));
	vec3 roundOffset = roundedOrigin - vec3(shadowOrigin.x, shadowOrigin.y, shadowOrigin.z);
	roundOffset = roundOffset * 2.0f / size;
	mat4 snapMat = projInit;
	snapMat.entries[12] += roundOffset.x;
	snapMat.entries[13] += roundOffset.y;
	return snapMat;
}

void Shadow::copyCameraData() {
	renderLightCameraDyn->copy(actLightCameraDyn);
	renderLightCameraNear->copy(actLightCameraNear);
	renderLightCameraMid->copy(actLightCameraMid);
	renderLightCameraFar->copy(actLightCameraFar);
}

void Shadow::mergeCamera() {
	if (renderLightCameraDyn && renderLightCameraDyn != actLightCameraDyn) {
		delete renderLightCameraDyn;
		renderLightCameraDyn = actLightCameraDyn;
	}
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
