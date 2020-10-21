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

	void updateLightCamera(Camera* lightCamera,Camera* actCamera,const vec4& center,float radius);
public:
	float distance1, distance2;
	Camera* actLightCameraNear;
	Camera* actLightCameraMid;
	Camera* actLightCameraFar;
	Camera* renderLightCameraNear;
	Camera* renderLightCameraMid;
	Camera* renderLightCameraFar;
	float level1,level2;
	float shadowMapSize,shadowPixSize,pixSize;
	vec3 lightDir;

	Shadow(Camera* view);
	~Shadow();

	void prepareViewCamera(float dist1, float dist2);
	void update(Camera* actCamera, const vec3& light);
	void copyCameraData();
	void mergeCamera();
};


#endif /* SHADOW_H_ */
