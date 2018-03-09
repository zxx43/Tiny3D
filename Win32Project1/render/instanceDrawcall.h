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
	bool indexed;
public:
	bool isSimple;
	int objectToDraw;
public:
	InstanceDrawcall(Instance* instance, bool simple);
	virtual ~InstanceDrawcall();
	virtual void createSimple();
	virtual void releaseSimple();
	virtual void draw(Shader* shader,int pass);
	void updateMatrices(const float* modelMatrices);
};

#endif /* INSTANCEDRAWCALL_H_ */
