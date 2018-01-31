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
	void createBatch();
public:
	Batch* batch;
	int batchVertexCount,batchIndexCount;
	bool fullStatic;

	StaticNode(const VECTOR3D& position);
	virtual ~StaticNode();
	virtual void addObject(Object* object);
	virtual Object* removeObject(Object* object);
	void addObjects(Object** objectArray,int count);
	virtual void prepareDrawcall();
	virtual void updateDrawcall(bool updateNormal);
};


#endif /* STATIC_NODE_H_ */
