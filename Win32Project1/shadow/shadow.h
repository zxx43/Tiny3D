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
	vec3* corners0;
	vec3* corners1;
	vec3* corners2;
	vec3* corners3;
	vec4 center0,center1,center2;
	float radius0,radius1,radius2;
	mat4 invViewMat;
private:
	void updateViewCamera(Camera* actCamera);
	void updateLightCamera(Camera* lightCamera, const vec4& center, float radius);
public:
	float distance1, distance2;
	Camera* actLightCameraDyn;
	Camera* actLightCameraNear;
	Camera* actLightCameraMid;
	Camera* actLightCameraFar;
	Camera* renderLightCameraDyn;
	Camera* renderLightCameraNear;
	Camera* renderLightCameraMid;
	Camera* renderLightCameraFar;
	float level1,level2;
	float shadowMapSize,shadowPixSize,pixSize;
	vec3 lightDir;
	bool flushDyn, flushNear, flushMid, flushFar;
	float gap, inv2Gap, radius;
public:
	Shadow(Camera* view);
	~Shadow();
public:
	void prepareViewCamera(float dist1, float dist2);
	void update(Camera* actCamera, const vec3& light);
	void copyCameraData();
	void mergeCamera();
	void setFlushDyn(bool f) { flushDyn = f; }
	void setFlushNear(bool f) { flushNear = f; }
	void setFlushMid(bool f) { flushMid = f; }
	void setFlushFar(bool f) { flushFar = f; }
};


#endif /* SHADOW_H_ */
