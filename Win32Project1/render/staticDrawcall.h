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
	StaticDrawcall();
	StaticDrawcall(Batch* batch);
	virtual ~StaticDrawcall();
	virtual void draw(Shader* shader);
	void updateMatrices(Batch* batch, bool updateNormals);
};


#endif /* INDEXEDDRAWCALL_H_ */
