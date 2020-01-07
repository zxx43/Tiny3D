/*
 * instanceDrawcall.h
 *
 *  Created on: 2017-9-29
 *      Author: a
 */

#ifndef INSTANCEDRAWCALL_H_
#define INSTANCEDRAWCALL_H_

class Instance;

#include "drawcall.h"

class InstanceDrawcall: public Drawcall {
private:
	int vertexCount,indexCount;
	Instance* instanceRef;
	Indirect* indirectBuf;
	Indirect* readBuf;
public:
	int objectToPrepare;
private:
	RenderBuffer* createBuffers(Instance* instance, int vertexCount, int indexCount);
public:
	InstanceDrawcall(Instance* instance);
	virtual ~InstanceDrawcall();
	virtual void draw(Render* render, RenderState* state, Shader* shader);
	void updateInstances(Instance* instance, int pass);
};

#endif /* INSTANCEDRAWCALL_H_ */
