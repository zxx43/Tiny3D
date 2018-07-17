#include "renderQueue.h"
#include "../node/staticNode.h"
#include "../node/animationNode.h"
#include "../node/instanceNode.h"
#include "../node/waterNode.h"
#include <string.h>
#include <stdlib.h>
using namespace std;

RenderQueue::RenderQueue(float midDis, float lowDis) {
	queue.clear();
	instanceQueue.clear();
	batchQueue.clear();
	midDistSqr = midDis * midDis;
	lowDistSqr = lowDis * lowDis;
	midDistZ = midDis;
	lowDistZ = lowDis;
}

RenderQueue::~RenderQueue() {
	queue.clear();

	for (uint i = 0; i < batchQueue.size(); ++i)
		delete batchQueue[i];
	batchQueue.clear();

	map<Mesh*, InstanceData*>::iterator itIns;
	for (itIns = instanceQueue.begin(); itIns != instanceQueue.end(); ++itIns)
		delete itIns->second;
	instanceQueue.clear();
}

void RenderQueue::push(Node* node) {
	queue.push_back(node);
}

void RenderQueue::flush() {
	queue.clear();

	map<Mesh*, InstanceData*>::iterator itData = instanceQueue.begin();
	while (itData != instanceQueue.end()) {
		itData->second->resetInstance();
		++itData;
	}
}

void RenderQueue::deleteInstance(InstanceData* data) {
	if (data->instance)
		delete data->instance;
	data->instance = NULL;
}

void RenderQueue::pushDatasToInstance(InstanceData* data) {
	if (data->count <= 0) return;
	Mesh* mesh = data->insMesh;
	Instance* instance = data->instance;
	if (!instance) {
		instance = new Instance(mesh);
		instance->initInstanceBuffers(data->object, mesh->vertexCount, mesh->indexCount);
		instance->singleSide = data->singleSide;
		data->instance = instance;
	}
	instance->setInstanceCount(data->count);
	instance->setRenderData(data->matrices, data->billboards, data->positions);
}

void RenderQueue::pushObjectToBatch(Object* object, Camera* camera, const VECTOR3D& eye) {
	if (object->checkInCamera(camera)) {
		Mesh* mesh = object->mesh;
		if ((eye - object->bounding->position).GetSquaredLength() > lowDistSqr) {
			if (object->meshLow) mesh = object->meshLow;
			else if (object->meshMid) mesh = object->meshMid;
		} else if ((eye - object->bounding->position).GetSquaredLength() > midDistSqr) {
			if (object->meshMid) mesh = object->meshMid;
		}
		Batch* batch;
		if (batchQueue.size() <= 0) {
			batch = new Batch(); batch->initBatchBuffers(MAX_VERTEX_COUNT, MAX_INDEX_COUNT);
			batchQueue.push_back(batch);
		} else
			batch = batchQueue[0];
		int currentObject = batch->objectCount;
		batch->pushMeshToBuffers(mesh, object->material, false, object->transformMatrix, object->normalMatrix);
		batch->updateMatrices(currentObject, object->transformMatrix, NULL);
	}
}

void RenderQueue::draw(Camera* camera, const VECTOR3D& eye, Render* render, RenderState* state) {
	vector<Node*>::iterator it = queue.begin();
	while (it != queue.end()) {
		Node* node = *it;
		if (!node->needUpdateNode) {
			if (node->needCreateDrawcall) 
				node->prepareDrawcall();
			else if (node->needUpdateDrawcall) {
				node->updateRenderData();
				node->updateDrawcall();
			}
		}

		if (node->type == TYPE_STATIC) {
			StaticNode* staticNode = (StaticNode*)node;
			if (staticNode->isDynamicBatch()) {
				for (uint i = 0; i < node->objects.size(); ++i) {
					Object* object = node->objects[i];
					pushObjectToBatch(object, camera, eye);
				}
			} else
				render->draw(camera, node->drawcall, state);
		} else if (node->type == TYPE_ANIMATE) {
			node->drawcall->uModelMatrix = node->uTransformMatrix->entries;

			if (!node->drawcall->uNormalMatrix)
				node->drawcall->uNormalMatrix = (float*)malloc(9 * sizeof(float));
			memcpy(node->drawcall->uNormalMatrix, node->uNormalMatrix->entries, 3 * sizeof(float));
			memcpy(node->drawcall->uNormalMatrix + 3, node->uNormalMatrix->entries + 4, 3 * sizeof(float));
			memcpy(node->drawcall->uNormalMatrix + 6, node->uNormalMatrix->entries + 8, 3 * sizeof(float));
			
			render->draw(camera, node->drawcall, state);
		} else if (node->type == TYPE_TERRAIN) {
			if (state->pass == 4) {
				static Shader* terrainShader = render->findShader("terrain");
				Shader* shader = state->shader;
				state->shader = terrainShader;
				render->draw(camera, node->drawcall, state);
				state->shader = shader;
			}
		} 

		++it;
	}
	
	map<Mesh*, InstanceData*>::iterator itData = instanceQueue.begin();
	while (itData != instanceQueue.end()) {
		InstanceData* data = itData->second;
		pushDatasToInstance(data);

		Instance* instance = data->instance;
		if (instance) {
			if (!instance->drawcall) instance->createDrawcall();
			if (data->count > 0 && (instance->modelMatrices || instance->billboards)) {
				instance->drawcall->updateInstances(instance, state->pass);
				render->draw(camera, instance->drawcall, state);
			}
		}
		++itData;
		if (Instance::instanceTable[instance->instanceMesh] == 0)
			deleteInstance(data);
	}	

	vector<Batch*>::iterator itBch = batchQueue.begin();
	while (itBch != batchQueue.end()) {
		Batch* batch = *itBch;
		if (!batch->drawcall) batch->createDrawcall();
		if (batch->objectCount > 0) {
			batch->drawcall->updateMatrices();
			batch->drawcall->updateBuffers(state->pass);
			render->draw(camera, batch->drawcall, state);
			batch->flushBatchBuffers();
		}
		++itBch;
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
		++it;
	}
}

void pushNodeToQueue(RenderQueue* queue, Node* node, Camera* camera, const VECTOR3D& eye) {
	if (node->checkInCamera(camera)) {
		for (unsigned int i = 0; i<node->children.size(); ++i) {
			Node* child = node->children[i];
			if (child->objects.size() <= 0)
				pushNodeToQueue(queue, child, camera, eye);
			else {
				if (child->checkInCamera(camera)) {
					if (child->type!= TYPE_INSTANCE)
						queue->push(child);
					else if (child->type == TYPE_INSTANCE) {
						child->needCreateDrawcall = false;
						child->needUpdateDrawcall = false;
						for (uint j = 0; j < child->objects.size(); j++) {
							Object* object = child->objects[j];
							if (object->checkInCamera(camera)) {
								Mesh* mesh = object->mesh;
								if ((eye - object->bounding->position).GetSquaredLength() > queue->lowDistSqr) {
									if (object->meshLow) mesh = object->meshLow;
									else if (object->meshMid) mesh = object->meshMid;
								} else if ((eye - object->bounding->position).GetSquaredLength() > queue->midDistSqr) {
									if (object->meshMid) mesh = object->meshMid;
								}


								InstanceData* insData = NULL;
								map<Mesh*, InstanceData*>::iterator itres = queue->instanceQueue.find(mesh);
								if (itres != queue->instanceQueue.end())
									insData = itres->second;
								else {
									insData = new InstanceData(mesh, object, child->singleSide);
									queue->instanceQueue[mesh] = insData;
								}
								insData->addInstance(object);
							}
						}
					}
				}
			}	
		}
	}
}
