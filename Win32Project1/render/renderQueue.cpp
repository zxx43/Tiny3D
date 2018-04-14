#include "renderQueue.h"
#include "../node/animationNode.h"
#include "../node/instanceNode.h"
#include <string.h>
using namespace std;

RenderQueue::RenderQueue() {
	queue.clear();
	instanceQueue.clear();
}

RenderQueue::~RenderQueue() {
	queue.clear();

	map<Mesh*, Instance*>::iterator itIns;
	for (itIns = instanceQueue.begin(); itIns != instanceQueue.end(); itIns++)
		delete itIns->second;
	instanceQueue.clear();
}

void RenderQueue::copyData(RenderQueue* src) {
	queue.assign(src->queue.begin(), src->queue.end());
}

void RenderQueue::push(Node* node) {
	queue.push_back(node);
}

void RenderQueue::flush() {
	queue.clear();
}

void RenderQueue::deleteInstance(Mesh* mesh) {
	map<Mesh*, Instance*>::iterator itIns = instanceQueue.find(mesh);
	if (itIns != instanceQueue.end()) {
		delete itIns->second;
		instanceQueue.erase(itIns);
	}
}

void RenderQueue::pushObjectToInstance(Object* object, Camera* camera, bool singleSide) {
	if (object->checkInCamera(camera)) {
		Mesh* mesh = object->mesh;
		if (instanceQueue.find(mesh) == instanceQueue.end()) {
			instanceQueue[mesh] = new Instance(mesh);
			instanceQueue[mesh]->initInstanceBuffers(object->material, mesh->vertexCount, mesh->indexCount);
			instanceQueue[mesh]->singleSide = singleSide;
		}
		Instance* instance = instanceQueue[mesh];
		int currentIns = instance->instanceCount;
		instance->setInstanceCount(currentIns + 1);
		instance->updateMatricesBuffer(currentIns, object->transformMatrix);
	}
}

void RenderQueue::draw(Camera* camera, Render* render, RenderState* state) {
	vector<Node*>::iterator it = queue.begin();
	while (it != queue.end()) {
		Node* node = *it;
		if (!node->needUpdateNode) {
			if (!node->drawcall || node->needCreateDrawcall)
				node->prepareDrawcall();
			else if (node->needUpdateDrawcall) {
				node->updateRenderData(camera, state->pass);
				node->updateDrawcall(state->pass);
			}
		}

		if (node->type == TYPE_INSTANCE) {
			for (uint i = 0; i < node->objects.size(); i++) {
				Object* object = node->objects[i];
				pushObjectToInstance(object, camera, node->singleSide);
			}
		} else if (node->drawcall) {
			if (node->type == TYPE_ANIMATE) {
				if (!node->drawcall->uModelMatrix)
					node->drawcall->uModelMatrix = new float[16];
				memcpy(node->drawcall->uModelMatrix, node->uTransformMatrix->entries, 16 * sizeof(float));
				/*
				for (int m = 0; m < 4; m++) {
					node->drawcall->uModelMatrix[m] = node->uTransformMatrix->entries[m];
					node->drawcall->uModelMatrix[m + 4] = node->uTransformMatrix->entries[m + 4];
					node->drawcall->uModelMatrix[m + 8] = node->uTransformMatrix->entries[m + 8];
					node->drawcall->uModelMatrix[m + 12] = node->uTransformMatrix->entries[m + 12];
				}
				*/
				if (!node->drawcall->uNormalMatrix)
					node->drawcall->uNormalMatrix = new float[9];
				memcpy(node->drawcall->uNormalMatrix, node->uNormalMatrix->entries, 3 * sizeof(float));
				memcpy(node->drawcall->uNormalMatrix + 3, node->uNormalMatrix->entries + 4, 3 * sizeof(float));
				memcpy(node->drawcall->uNormalMatrix + 6, node->uNormalMatrix->entries + 8, 3 * sizeof(float));
				/*
				for (int m = 0; m < 3; m++) {
					node->drawcall->uNormalMatrix[m] = node->uNormalMatrix->entries[m];
					node->drawcall->uNormalMatrix[m + 3] = node->uNormalMatrix->entries[m + 4];
					node->drawcall->uNormalMatrix[m + 6] = node->uNormalMatrix->entries[m + 8];
				}
				*/
			}
			render->draw(camera, node->drawcall, state);
		}

		it++;
	}

	map<Mesh*, Instance*>::iterator itIns = instanceQueue.begin();
	while (itIns != instanceQueue.end()) {
		Instance* instance = itIns->second;
		if (!instance->drawcall) instance->createDrawcall(state->pass != 4);
		if (instance->instanceCount > 0 && instance->modelMatrices) {
			instance->drawcall->updateMatrices(instance->modelMatrices);
			render->draw(camera, instance->drawcall, state);
			instance->setInstanceCount(0);
		}
		itIns++;
		if (Instance::instanceTable[instance->instanceMesh] == 0)
			deleteInstance(instance->instanceMesh);
	}	
}

void RenderQueue::animate(long startTime, long currentTime) {
	vector<Node*>::iterator it = queue.begin();
	while(it!=queue.end()) {
		Node* node = *it;
		if (node->type == TYPE_ANIMATE) {
			AnimationNode* animateNode = (AnimationNode*)node;
			animateNode->animate(0, startTime, currentTime);
		}
		it++;
	}
}

void pushNodeToQueue(RenderQueue* queue, Node* node, Camera* camera) {
	if (node->checkInCamera(camera)) {
		for (unsigned int i = 0; i<node->children.size(); i++) {
			Node* child = node->children[i];
			if (child->objects.size()>0) {
				if (child->checkInCamera(camera)) {
					queue->push(child);
				}
			}
			else
				pushNodeToQueue(queue, child, camera);
		}
	}
}
