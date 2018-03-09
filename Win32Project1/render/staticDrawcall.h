/*
 * indexedDrawcall.h
 *
 *  Created on: 2017-7-28
 *      Author: a
 */

#ifndef INDEXEDDRAWCALL_H_
#define INDEXEDDRAWCALL_H_

#include "drawcall.h"
#include "../batch/batch.h"

class StaticDrawcall: public Drawcall {
private:
	int vertexCount,indexCount;
	bool indexed;
public:
	Batch* batch;
	StaticDrawcall();
	StaticDrawcall(Batch* batch);
	virtual ~StaticDrawcall();
	virtual void createSimple();
	virtual void releaseSimple();
	virtual void draw(Shader* shader,int pass);
	void updateMatrices(Batch* batch);
};


#endif /* INDEXEDDRAWCALL_H_ */
