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
	bool isGroup;
public:
	bool dynamic;
	InstanceData* groupBuffer;
	InstanceState* insState;
public:
	InstanceNode(const VECTOR3D& position);
	virtual ~InstanceNode();
	void addObjects(Object** objectArray,int count);
	void prepareGroup();
	void releaseGroup();
	void setSingle(bool single) { singleSide = single; insState->singleSide = single; };
	void setGroup(bool group) { isGroup = group; };
	void setSimple(bool simp) { insState->simple = simp; };
	void setGrass(bool grass) { insState->grass = grass; };
	bool getGroup() { return isGroup; };
	bool getSimple() { return insState->simple; };
	bool getGrass() { return insState->grass; };
	virtual void addObject(Object* object);
	virtual Object* removeObject(Object* object);
	virtual void prepareDrawcall();
	virtual void updateRenderData();
	virtual void updateDrawcall();
};


#endif /* INSTANCENODE_H_ */
