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
	bool doubleBuffer;
	RenderBuffer* dataBuffer2;

	RenderBuffer* dataBufferDraw;
	RenderBuffer* dataBufferPrepare;
	bool dynDC;
private:
	int objectToDraw;
public:
	int objectToPrepare;
	bool isSimple, isGrass;
private:
	RenderBuffer* createBuffers(Instance* instance, bool dyn, int vertexCount, int indexCount, RenderBuffer* dupBuf);
public:
	InstanceDrawcall(Instance* instance);
	virtual ~InstanceDrawcall();
	virtual void draw(Render* render, RenderState* state, Shader* shader);
	void updateInstances(Instance* instance, int pass);
};

#endif /* INSTANCEDRAWCALL_H_ */
