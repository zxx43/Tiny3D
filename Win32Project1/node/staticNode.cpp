#include "staticNode.h"
#include "../render/staticDrawcall.h"
#include "../util/util.h"

StaticNode::StaticNode(const VECTOR3D& position):Node(position, VECTOR3D(0, 0, 0)) {
	batch = NULL;
	dynamicBatch = true;
	fullStatic = false;
	type = TYPE_STATIC;
}

StaticNode::~StaticNode() {
	if(batch)
		delete batch;
	batch=NULL;
}

void StaticNode::addObjects(Object** objectArray,int count) {
	for(int i=0;i<count;i++)
		addObject(objectArray[i]);
}

void StaticNode::createBatch() {
	if (batch) delete batch;
	batch = new Batch();

	int vertCount = 0, indCount = 0;
	for (uint i = 0; i < objects.size(); i++) {
		vertCount += objects[i]->mesh->vertexCount;
		indCount += objects[i]->mesh->indexCount;
	}
	batch->initBatchBuffers(vertCount, indCount);
	batch->setDynamic(false);
	
	for(uint i=0;i<objects.size();i++) {
		Object* object=objects[i];
		batch->pushMeshToBuffers(object->mesh,object->material,fullStatic,object->transformMatrix,object->normalMatrix);
	}

	if (drawcall) delete drawcall;
	drawcall=new StaticDrawcall(batch);
	drawcall->setSide(singleSide);
}

void StaticNode::prepareDrawcall() {
	if (!dynamicBatch) createBatch();
	needCreateDrawcall = false;
}

void StaticNode::updateRenderData() {
	if (dynamicBatch) return;

	for (unsigned int i = 0; i < objects.size(); i++) {
		Object* object = objects[i];
		batch->updateMatrices(i, object->transformMatrix, NULL);
	}
}

void StaticNode::updateDrawcall() {
	if (!dynamicBatch && drawcall) 
		((StaticDrawcall*)drawcall)->updateMatrices();
	needUpdateDrawcall = false;
}

bool StaticNode::isFullStatic() {
	return fullStatic;
}

void StaticNode::setFullStatic(bool fullStatic) {
	this->fullStatic = fullStatic;
	if(fullStatic) setDynamicBatch(false);
}

bool StaticNode::isDynamicBatch() {
	return dynamicBatch;
}

void StaticNode::setDynamicBatch(bool dynamic) {
	dynamicBatch = dynamic;
}
