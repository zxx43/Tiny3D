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
	Instance* instance;
public:
	InstanceNode(const vec3& position);
	virtual ~InstanceNode();
	void addObjects(Scene* scene, Object** objectArray, int count);
	virtual void addObject(Scene* scene, Object* object);
	virtual Object* removeObject(Scene* scene, Object* object);
	virtual void prepareDrawcall();
	virtual void updateRenderData() {}
	virtual void updateDrawcall() { needUpdateDrawcall = false; }
};


#endif /* INSTANCENODE_H_ */
