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

	void updateLightCamera(Camera* lightCamera,const vec4* center,float radius);
public:
	float distance1, distance2;
	Camera* lightCameraNear;
	Camera* lightCameraMid;
	Camera* lightCameraFar;
	mat4 lightNearMat, lightMidMat, lightFarMat;
	float level1,level2;
	float shadowMapSize,shadowPixSize;
	vec3 lightDir;

	Shadow(Camera* view);
	~Shadow();

	void prepareViewCamera(float dist1, float dist2);
	void update(const vec3& light);
};


#endif /* SHADOW_H_ */
