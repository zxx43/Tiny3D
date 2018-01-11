/*
 * animationDrawcall.h
 *
 *  Created on: 2017-6-24
 *      Author: a
 */

#ifndef ANIMATIONDRAWCALL_H_
#define ANIMATIONDRAWCALL_H_

#include "drawcall.h"
#include "../animation/animation.h"

#define UNIFORM_BONEMATS "boneMats"

class AnimationDrawcall: public Drawcall {
private:
	float* vertices;
	float* normals;
	unsigned char* texcoords;
	short* textureids;
	unsigned char* colors;
	unsigned short* boneids;
	float* weights;
	unsigned int* indices;
	int indexCount;
	int textureChannel;

	int boneCount;
	float* boneMatrices;
public:
	AnimationDrawcall();
	AnimationDrawcall(Animation* animation);
	virtual ~AnimationDrawcall();
	virtual void draw(Shader* shader);
};

#endif /* ANIMATIONDRAWCALL_H_ */
