/*
 * indexedDrawcall.h
 *
 *  Created on: 2017-7-28
 *      Author: a
 */

#ifndef INDEXEDDRAWCALL_H_
#define INDEXEDDRAWCALL_H_

class Batch;

#include "drawcall.h"

class StaticDrawcall: public Drawcall {
private:
	int vertexCount, indexCount;
	bool indexed;
	Batch* batchRef;

	RenderBuffer* dataBuffer2;
	RenderBuffer* bufferToDraw;
	RenderBuffer* bufferToPrepare;

	int vertexCntToDraw, indexCntToDraw, objectCntToDraw;
	float* modelMatricesToPrepare;
	bool dynDC, doubleBuffer;
public:
	int vertexCntToPrepare, indexCntToPrepare, objectCntToPrepare;
private:
	RenderBuffer* createBuffers(Batch* batch, int bufCount, int vertCount, int indCount, GLenum drawType);
	void flushMatricesToPrepare();
public:
	StaticDrawcall(Batch* batch);
	virtual ~StaticDrawcall();
	virtual void draw(Shader* shader,int pass);
	void updateMatrices();
	void updateBuffers(int pass);
};


#endif /* INDEXEDDRAWCALL_H_ */
