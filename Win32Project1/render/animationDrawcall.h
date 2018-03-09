/*
 * animationDrawcall.h
 *
 *  Created on: 2017-6-24
 *      Author: a
 */

#ifndef ANIMATIONDRAWCALL_H_
#define ANIMATIONDRAWCALL_H_

#include "drawcall.h"
#include "../animation/animationData.h"

#define UNIFORM_BONEMATS "boneMats"

class AnimationDrawcall: public Drawcall {
private:
	int vertexCount, indexCount;
	AnimationData* animData;
public:
	AnimationDrawcall();
	AnimationDrawcall(Animation* anim);
	virtual ~AnimationDrawcall();
	virtual void createSimple();
	virtual void releaseSimple();
	virtual void draw(Shader* shader,int pass);
};

#endif /* ANIMATIONDRAWCALL_H_ */
