/*
 * node.h
 *
 *  Created on: 2017-4-2
 *      Author: a
 */

#ifndef STATIC_NODE_H_
#define STATIC_NODE_H_

#include "node.h"
#include "../batch/batch.h"

class StaticNode: public Node {
private:
	bool fullStatic;
	bool dynamicBatch;
private:
	void createBatch();
public:
	Batch* batch;

	StaticNode(const VECTOR3D& position);
	virtual ~StaticNode();
	void addObjects(Object** objectArray,int count);
	virtual void prepareDrawcall();
	virtual void updateRenderData();
	virtual void updateDrawcall();
	bool isFullStatic();
	void setFullStatic(bool fullStatic);
	bool isDynamicBatch();
	void setDynamicBatch(bool dynamic);
};


#endif /* STATIC_NODE_H_ */
