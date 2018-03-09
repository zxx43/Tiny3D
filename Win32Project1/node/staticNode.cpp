#include "staticNode.h"
#include "../render/staticDrawcall.h"
#include "../util/util.h"

StaticNode::StaticNode(const VECTOR3D& position):Node(position, VECTOR3D(0, 0, 0)) {
	batch = NULL;
	batchVertexCount = 0;
	batchIndexCount = 0;
	fullStatic = false;
	type = TYPE_STATIC;
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
	
	for(unsigned int i=0;i<objects.size();i++) {
		Object* object=objects[i];
		batch->pushMeshToBuffers(object->mesh,object->material,fullStatic,object->transformMatrix,object->normalMatrix);
	}

	if (drawcall) delete drawcall;
	drawcall=new StaticDrawcall(batch);
	drawcall->setSide(singleSide);
}

void StaticNode::prepareDrawcall() {
	createBatch();
	needCreateDrawcall = false;
}

void StaticNode::updateRenderData(Camera* camera, int pass) {
	if (!batch) return;

	for (unsigned int i = 0; i < objects.size(); i++) {
		Object* object = objects[i];
		batch->updateMatrices(i, object->transformMatrix, NULL);
	}
}

void StaticNode::updateDrawcall(int pass) {
	if (!batch) return;

	if (!drawcall) {
		drawcall = new StaticDrawcall(batch);
		drawcall->setSide(singleSide);
	} else
		((StaticDrawcall*)drawcall)->updateMatrices(batch);

	needUpdateDrawcall = false;
}

