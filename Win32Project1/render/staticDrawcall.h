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
	int vertexCount,indexCount;
	bool indexed;
public:
	Batch* batch;
	StaticDrawcall(Batch* batch);
	virtual ~StaticDrawcall();
	virtual void draw(Shader* shader,int pass);
	void updateMatrices();
	void updateBuffers(int pass);
};


#endif /* INDEXEDDRAWCALL_H_ */
