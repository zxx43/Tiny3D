#include "staticNode.h"
#include "../render/staticDrawcall.h"
#include "../util/util.h"

StaticNode::StaticNode(const VECTOR3D& position):Node(position, VECTOR3D(0, 0, 0)) {
	batch = NULL;
	batchVertexCount = 0;
	batchIndexCount = 0;
	fullStatic = false;
}

StaticNode::~StaticNode() {
	if(batch)
		delete batch;
	batch=NULL;
}

void StaticNode::addObject(Object* object) {
	batchVertexCount+=object->mesh->vertexCount;
	batchIndexCount+=object->mesh->indexCount;
	Node::addObject(object);
}

Object* StaticNode::removeObject(Object* object) {
	if(!object) return NULL;
	int beforeVertexCount=batchVertexCount;
	int beforeIndexCount=batchIndexCount;
	batchVertexCount-=object->mesh->vertexCount;
	batchIndexCount-=object->mesh->indexCount;
	Object* objectDel=Node::removeObject(object);
	if(!objectDel) { // Rollback
		batchVertexCount=beforeVertexCount;
		batchIndexCount=beforeIndexCount;
	}
	return objectDel;
}

void StaticNode::addObjects(Object** objectArray,int count) {
	for(int i=0;i<count;i++)
		addObject(objectArray[i]);
}

void StaticNode::createBatch() {
	if (batch) delete batch;
	batch = new Batch();
	batch->initBatchBuffers(batchVertexCount,batchIndexCount);
	MATRIX4X4 nodeTransform; nodeTransform.LoadIdentity();
	recursiveTransform(nodeTransform);
	for(unsigned int i=0;i<objects.size();i++) {
		Object* object=objects[i];
		MATRIX4X4 transformMatrix=nodeTransform*object->localTransformMatrix;
		batch->pushMeshToBuffers(object->mesh,object->material,fullStatic,transformMatrix,object->normalMatrix);
	}

	if (drawcall) delete drawcall;
	drawcall=new StaticDrawcall(batch);
	drawcall->setSide(singleSide);
}

void StaticNode::prepareDrawcall() {
	createBatch();
	needCreateDrawcall = false;
}

void StaticNode::updateDrawcall(bool updateNormal) {
	if (!batch) createBatch();
	else {
		MATRIX4X4 nodeTransform; nodeTransform.LoadIdentity();
		recursiveTransform(nodeTransform);
		int baseVertex = 0;
		for (unsigned int i = 0; i < objects.size(); i++) {
			Object* object = objects[i];
			MATRIX4X4 transformMatrix = nodeTransform * object->localTransformMatrix;
			if (updateNormal) {
				MATRIX4X4 normalMatrix = object->normalMatrix;
				batch->updateMatricesBuffer(baseVertex, object->mesh, transformMatrix, &normalMatrix);
			} else
				batch->updateMatricesBuffer(baseVertex, object->mesh, transformMatrix, NULL);

			baseVertex += object->mesh->vertexCount;
		}

		if (!drawcall) {
			drawcall = new StaticDrawcall(batch);
			drawcall->setSide(singleSide);
		} else 
			((StaticDrawcall*)drawcall)->updateMatrices(batch, updateNormal);
	}
	needUpdateDrawcall = false;
}

