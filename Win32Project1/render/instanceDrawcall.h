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
	int objectToDraw;
public:
	InstanceDrawcall(Instance* instance);
	virtual ~InstanceDrawcall();
	virtual void draw(Shader* shader,int pass);
	void updateInstances(Instance* instance, int pass);
};

#endif /* INSTANCEDRAWCALL_H_ */
