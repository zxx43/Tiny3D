/*
 * sky.h
 *
 *  Created on: 2017-9-19
 *      Author: a
 */

#ifndef SKY_H_
#define SKY_H_

#include "../mesh/sphere.h"
#include "../node/staticNode.h"
#include "../render/render.h"
#include "../camera/camera.h"

class Scene;

class Sky {
private:
	StaticNode* skyNode;
	Sphere* mesh;
	RenderState* state;
	FrameBuffer* skyBuff;
	mat4 matPosx, matNegx, matPosy, matNegy, matPosz, matNegz;
public:
	Sky(Scene* scene);
	~Sky();
public:
	void update(Render* render, const vec3& sunPos, Shader* shader);
	void draw(Render* render,Shader* shader,Camera* camera);
};


#endif /* SKY_H_ */
