#include "staticNode.h"
#include "../render/staticDrawcall.h"
#include "../render/terrainDrawcall.h"
#include "../util/util.h"
#include "../scene/scene.h"

StaticNode::StaticNode(const vec3& position):Node(position, vec3(0, 0, 0)) {
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

void StaticNode::addObjects(Scene* scene,Object** objectArray,int count) {
	for(int i=0;i<count;i++)
		addObject(scene,objectArray[i]);
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
	if (!batch->hasTerrain)
		drawcall = new StaticDrawcall(batch);
	else
		drawcall = new TerrainDrawcall((Terrain*)(objects[0]->mesh), batch);
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
	if (!dynamicBatch && drawcall && !batch->hasTerrain) 
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
