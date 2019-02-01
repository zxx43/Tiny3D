#include "renderQueue.h"
#include "../node/staticNode.h"
#include "../node/animationNode.h"
#include "../node/instanceNode.h"
#include "../assets/assetManager.h"
#include <string.h>
#include <stdlib.h>
using namespace std;

RenderQueue::RenderQueue(float midDis, float lowDis) {
	queue = new Queue(128);
	instanceQueue.clear();
	batchData = NULL;
	midDistSqr = midDis * midDis;
	lowDistSqr = lowDis * lowDis;
	dual = true;
	shadowLevel = 0;
}

RenderQueue::~RenderQueue() {
	delete queue;

	if (batchData)
		delete batchData;

	map<Mesh*, InstanceData*>::iterator itIns;
	for (itIns = instanceQueue.begin(); itIns != instanceQueue.end(); ++itIns)
		delete itIns->second;
	instanceQueue.clear();
}

void RenderQueue::setDual(bool dual) {
	this->dual = dual;
}

void RenderQueue::push(Node* node) {
	queue->push(node);
}

void RenderQueue::flush() {
	queue->flush();
	
	map<Mesh*, InstanceData*>::iterator itData = instanceQueue.begin();
	while (itData != instanceQueue.end()) {
		itData->second->resetInstance();
		++itData;
	}
	
	if (batchData)
		batchData->resetBatch();
}

void RenderQueue::deleteInstance(InstanceData* data) {
	if (data->instance)
		delete data->instance;
	data->instance = NULL;
}

void RenderQueue::pushDatasToInstance(InstanceData* data, bool copy) {
	if (data->count <= 0) return;
	if (!data->instance) {
		data->instance = new Instance(data->insMesh, true, data->simpleTransform);
		data->instance->initInstanceBuffers(data->object, data->insMesh->vertexCount, data->insMesh->indexCount, MAX_INSTANCE_COUNT, copy);
		data->instance->singleSide = data->singleSide;
	}
	data->instance->setRenderData(data);
}

void RenderQueue::pushDatasToBatch(BatchData* data, int pass) {
	if (data->objectCount <= 0) return;
	if (!data->batch) {
		data->batch = new Batch(); 
		data->batch->initBatchBuffers(MAX_VERTEX_COUNT, MAX_INDEX_COUNT);
		data->batch->setDynamic(true);
	}
	data->batch->setRenderData(pass, data);
}

void RenderQueue::draw(Camera* camera, Render* render, RenderState* state) {
	for (int it = 0; it < queue->size; it++) {
		Node* node = queue->get(it);
		if (!node->needUpdateNode) {
			if (node->needCreateDrawcall) 
				node->prepareDrawcall();
			else if (node->needUpdateDrawcall) {
				node->updateRenderData();
				node->updateDrawcall();
			}
		}

		if (node->type == TYPE_STATIC) 
			render->draw(camera, node->drawcall, state);
		else if (node->type == TYPE_INSTANCE) {
			InstanceNode* instanceNode = (InstanceNode*)node;
			if (!instanceNode->dynamic) {
				state->simpleIns = instanceNode->getSimple();
				render->draw(camera, node->drawcall, state);
				state->simpleIns = false;
			}
		} else if (node->type == TYPE_ANIMATE) {
			node->drawcall->uModelMatrix = node->uTransformMatrix->entries;

			if (!node->drawcall->uNormalMatrix)
				node->drawcall->uNormalMatrix = (float*)malloc(9 * sizeof(float));
			memcpy(node->drawcall->uNormalMatrix, node->uNormalMatrix->entries, 3 * sizeof(float));
			memcpy(node->drawcall->uNormalMatrix + 3, node->uNormalMatrix->entries + 4, 3 * sizeof(float));
			memcpy(node->drawcall->uNormalMatrix + 6, node->uNormalMatrix->entries + 8, 3 * sizeof(float));

			render->draw(camera, node->drawcall, state);
		} else if (node->type == TYPE_TERRAIN) {
			if (state->pass == COLOR_PASS) {
				static Shader* terrainShader = render->findShader("terrain");
				Shader* shader = state->shader;
				state->shader = terrainShader;
				render->useTexture(TEXTURE_2D_ARRAY, 0, AssetManager::assetManager->texArray->setId);
				render->draw(camera, node->drawcall, state);
				render->useTexture(TEXTURE_2D, 0, AssetManager::assetManager->texAlt->texId);
				state->shader = shader;
			}
		}
	}

	map<Mesh*, InstanceData*>::iterator itData = instanceQueue.begin();
	while (itData != instanceQueue.end()) {
		InstanceData* data = itData->second;
		//pushDatasToInstance(data, dual);
		pushDatasToInstance(data, false);
		Instance* instance = data->instance;
		if (instance) {
			if (!instance->drawcall) instance->createDrawcall();
			if (data->count > 0 && (instance->modelMatrices || instance->billboards)) {
				instance->drawcall->updateInstances(instance, state->pass);
				state->simpleIns = instance->isSimple;
				render->draw(camera, instance->drawcall, state);
				state->simpleIns = false;
			}
		}
		++itData;
		if (instance && Instance::instanceTable[instance->instanceMesh] == 0)
			deleteInstance(data);
	}
	
	if (batchData) {
		pushDatasToBatch(batchData, state->pass);
		Batch* batch = batchData->batch;
		if (batch) {
			if (!batch->drawcall) batch->createDrawcall();
			if (batch->objectCount > 0) {
				batch->drawcall->updateBuffers(state->pass);
				batch->drawcall->updateMatrices();
				render->draw(camera, batch->drawcall, state);
			}
		}
	}
}

void RenderQueue::animate(long startTime, long currentTime) {
	for (int it = 0; it < queue->size; it++) {
		Node* node = queue->get(it);
		if (node->type == TYPE_ANIMATE) {
			AnimationNode* animateNode = (AnimationNode*)node;
			animateNode->animate(0, startTime, currentTime);
		}
	}
}

Mesh* RenderQueue::queryLodMesh(Object* object, const VECTOR3D& eye) {
	Mesh* mesh = object->mesh;
	float e2oDis = (eye - object->bounding->position).GetSquaredLength();
	if (e2oDis > lowDistSqr) 
		mesh = object->meshLow;
	else if (e2oDis > midDistSqr) 
		mesh = object->meshMid;
	
	return mesh;
}

void PushNodeToQueue(RenderQueue* queue, Node* node, Camera* camera, Camera* mainCamera) {
	if (node->checkInCamera(camera)) {
		for (unsigned int i = 0; i<node->children.size(); ++i) {
			Node* child = node->children[i];
			if (child->objects.size() <= 0)
				PushNodeToQueue(queue, child, camera, mainCamera);
			else {
				if (child->shadowLevel < queue->shadowLevel) continue;

				InstanceNode* insChild = NULL;
				if (child->type == TYPE_INSTANCE) {
					insChild = (InstanceNode*)child;
					if (insChild->getGroup()) {
						if (queue->shadowLevel > 0) {
							if (mainCamera->frustumNear && !insChild->checkInFrustum(mainCamera->frustumNear))
								continue;
						} else {
							if (mainCamera->frustumSub && !insChild->checkInFrustum(mainCamera->frustumSub))
								continue;
						}
					}
				}

				if (child->checkInCamera(camera)) {
					if (child->type != TYPE_INSTANCE && child->type != TYPE_STATIC)
						queue->push(child);
					else if (child->type == TYPE_STATIC) {
						if (!((StaticNode*)child)->isDynamicBatch())
							queue->push(child);
						else {
							child->needCreateDrawcall = false;
							child->needUpdateDrawcall = false;
							for (uint j = 0; j < child->objects.size(); ++j) {
								Object* object = child->objects[j];
								if (queue->shadowLevel > 0 && !object->genShadow) continue;
								if (object->checkInCamera(camera)) {
									Mesh* mesh = queue->queryLodMesh(object, mainCamera->position);
									if (!mesh) continue;
									if (!queue->batchData)
										queue->batchData = new BatchData();
									queue->batchData->addObject(object, mesh);
								}
							}
						}
					} else if (child->type == TYPE_INSTANCE && insChild) {
						if (!insChild->dynamic) {
							queue->push(insChild);
							continue;
						}
						
						insChild->needCreateDrawcall = false;
						insChild->needUpdateDrawcall = false;
						
						if (insChild->getGroup()) {
							Object* object = insChild->objects[0];
							Mesh* mesh = object->mesh;

							InstanceData* insData = NULL;
							map<Mesh*, InstanceData*>::iterator itres = queue->instanceQueue.find(mesh);
							if (itres != queue->instanceQueue.end())
								insData = itres->second;
							else {
								insData = new InstanceData(mesh, object, MAX_INSTANCE_COUNT, insChild->singleSide, insChild->getSimple());
								queue->instanceQueue.insert(pair<Mesh*, InstanceData*>(mesh, insData));
							}

							if (!insChild->groupBuffer)
								insChild->prepareGroup();
							insData->merge(insChild->groupBuffer);
						} else {
							for (uint j = 0; j < insChild->objects.size(); ++j) {
								Object* object = insChild->objects[j];
								if (queue->shadowLevel > 0 && !object->genShadow) continue;
								if (object->checkInCamera(camera)) {
									Mesh* mesh = queue->queryLodMesh(object, mainCamera->position);
									if (!mesh) continue;
									InstanceData* insData = NULL;
									map<Mesh*, InstanceData*>::iterator itres = queue->instanceQueue.find(mesh);
									if (itres != queue->instanceQueue.end())
										insData = itres->second;
									else {
										insData = new InstanceData(mesh, object, MAX_INSTANCE_COUNT, insChild->singleSide, insChild->getSimple());
										queue->instanceQueue.insert(pair<Mesh*, InstanceData*>(mesh, insData));
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
}