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
	Batch* batchRef;

	RenderBuffer* dataBuffer2;
	RenderBuffer* dataBufferVisual;
	RenderBuffer* bufferToDraw;
	RenderBuffer* bufferToPrepare;

	int vertexCntToDraw, indexCntToDraw, objectCntToDraw;
	float* modelMatricesToPrepare;
	bool dynDC, doubleBuffer;
private:
	int bufCount, vertCount, indCount;
	GLenum drawType;
public:
	int vertexCntToPrepare, indexCntToPrepare, objectCntToPrepare;
private:
	RenderBuffer* createBuffers(Batch* batch, int bufCount, int vertCount, int indCount, GLenum drawType, RenderBuffer* dupBuf);
	void flushMatricesToPrepare();
public:
	StaticDrawcall(Batch* batch);
	virtual ~StaticDrawcall();
	virtual void draw(Render* render, RenderState* state, Shader* shader);
	void updateMatrices();
	void updateBuffers(int pass, uint* indices = NULL, int indexCount = 0);
};


#endif /* INDEXEDDRAWCALL_H_ */
