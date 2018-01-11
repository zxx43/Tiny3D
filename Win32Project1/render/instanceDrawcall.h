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
	int vertexCount,indexCount,instanceCount;
	bool indexed;
public:
	InstanceDrawcall(Instance* instance);
	virtual ~InstanceDrawcall();
	virtual void draw(Shader* shader);
	void updateMatrices(Instance* instance, bool updateNormals);
};

#endif /* INSTANCEDRAWCALL_H_ */
