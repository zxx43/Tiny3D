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

struct Indirect {
	uint count;
	uint primCount;
	uint firstIndex;
	uint baseVertex;
	uint baseInstance;
};

class InstanceDrawcall: public Drawcall {
private:
	int vertexCount,indexCount;
	Instance* instanceRef;
	bool dynDC;
	Indirect* indirectBuf;
	Indirect* readBuf;
public:
	int objectToPrepare;
private:
	RenderBuffer* createBuffers(Instance* instance, bool dyn, int vertexCount, int indexCount);
public:
	InstanceDrawcall(Instance* instance);
	virtual ~InstanceDrawcall();
	virtual void draw(Render* render, RenderState* state, Shader* shader);
	void updateInstances(Instance* instance, int pass);
};

#endif /* INSTANCEDRAWCALL_H_ */
