/*
 * instanceNode.h
 *
 *  Created on: 2017-9-28
 *      Author: a
 */

#ifndef INSTANCENODE_H_
#define INSTANCENODE_H_

#include "node.h"
#include "../instance/instance.h"

class InstanceNode: public Node {
private:
	void createInstance();
public:
	Instance* instance;

	InstanceNode(const VECTOR3D& position);
	virtual ~InstanceNode();
	void addObjects(Object** objectArray,int count);
	virtual void prepareDrawcall();
	virtual void updateDrawcall(bool updateNormal);
};


#endif /* INSTANCENODE_H_ */
