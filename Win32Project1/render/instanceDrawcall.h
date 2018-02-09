/*
 * instanceDrawcall.h
 *
 *  Created on: 2017-9-29
 *      Author: a
 */

#ifndef INSTANCEDRAWCALL_H_
#define INSTANCEDRAWCALL_H_

#include "drawcall.h"
#include "../instance/instance.h"

class InstanceDrawcall: public Drawcall {
private:
	int vertexCount,indexCount;
	bool indexed;
	Instance* instance;
public:
	InstanceDrawcall(Instance* instance);
	virtual ~InstanceDrawcall();
	virtual void createSimple();
	virtual void releaseSimple();
	virtual void draw(Shader* shader,bool simple);
	void updateMatrices(Instance* instance, bool updateNormals);
};

#endif /* INSTANCEDRAWCALL_H_ */
