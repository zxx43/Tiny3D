/*
 * instanceNode.h
 *
 *  Created on: 2017-9-28
 *      Author: a
 */

#ifndef INSTANCENODE_H_
#define INSTANCENODE_H_

#include "node.h"
#include "../instance/instanceData.h"
#include "../instance/instance.h"

class InstanceNode: public Node {
private:
	Instance* instance;
	bool simple, isGroup;
public:
	bool dynamic;
	InstanceData* groupBuffer;
public:
	InstanceNode(const VECTOR3D& position);
	virtual ~InstanceNode();
	void addObjects(Object** objectArray,int count);
	void prepareGroup();
	void releaseGroup();
	void setGroup(bool group) { isGroup = group; };
	void setSimple(bool simp) { simple = simp; };
	bool getGroup() { return isGroup; };
	bool getSimple() { return simple; };
	virtual void addObject(Object* object);
	virtual Object* removeObject(Object* object);
	virtual void prepareDrawcall();
	virtual void updateRenderData();
	virtual void updateDrawcall();
};


#endif /* INSTANCENODE_H_ */
