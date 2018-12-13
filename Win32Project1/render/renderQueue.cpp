#include "renderQueue.h"
#include "../node/staticNode.h"
#include "../node/animationNode.h"
#include "../node/instanceNode.h"
#include "../assets/assetManager.h"
#include <string.h>
#include <stdlib.h>
using namespace std;

RenderQueue::RenderQueue(float midDis, float lowDis) {
	queue.clear();
	instanceQueue.clear();
	batchData = NULL;
	midDistSqr = midDis * midDis;
	lowDistSqr = lowDis * lowDis;
	midDistZ = midDis;
	lowDistZ = lowDis;
}

RenderQueue::~RenderQueue() {
	queue.clear();

	if (batchData)
		delete batchData;

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

	if (batchData)
		batchData->resetBatch();
}

void RenderQueue::deleteInstance(InstanceData* data) {
	if (data->instance)
		delete data->instance;
	data->instance = NULL;
}

void RenderQueue::pushDatasToInstance(InstanceData* data) {
	if (data->count <= 0) return;
	if (!data->instance) {
		data->instance = new Instance(data->insMesh);
		data->instance->initInstanceBuffers(data->object, data->insMesh->vertexCount, data->insMesh->indexCount);
		data->instance->singleSide = data->singleSide;
	}
	data->instance->setRenderData(data->count, data->matrices, data->billboards, data->positions);
}

void RenderQueue::pushDatasToBatch(BatchData* data, int pass) {
	if (data->objectCount <= 0) return;
	if (!data->batch) {
		data->batch = new Batch(); 
		data->batch->initBatchBuffers(MAX_VERTEX_COUNT, MAX_INDEX_COUNT);
		data->batch->setDynamic(true);
	}
	data->batch->setRenderData(pass, data->vertexCount, data->indexCount, data->objectCount,
		data->vertices, data->normals, data->texcoords, data->colors, data->objectids,
		data->indices, data->matrices);
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

		if (node->type == TYPE_STATIC) 
			render->draw(camera, node->drawcall, state);
		else if (node->type == TYPE_ANIMATE) {
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
		if (instance && Instance::instanceTable[instance->instanceMesh] == 0)
			deleteInstance(data);
	}
	
	if (batchData) {
		pushDatasToBatch(batchData, state->pass);
		Batch* batch = batchData->batch;
		if (batch) {
			if (!batch->drawcall) batch->createDrawcall();
			if (batch->objectCount > 0) {
				batch->drawcall->updateMatrices();
				batch->drawcall->updateBuffers(state->pass);
				render->draw(camera, batch->drawcall, state);
			}
		}
	}
}

void RenderQueue::animate(long startTime, long currentTime) {
	vector<Node*>::iterator it = queue.begin();
	while (it != queue.end()) {
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
								if (object->checkInCamera(camera)) {
									Mesh* mesh = object->mesh;
									if ((eye - object->bounding->position).GetSquaredLength() > queue->lowDistSqr) {
										if (object->meshLow) mesh = object->meshLow;
										else if (object->meshMid) mesh = object->meshMid;
									} else if ((eye - object->bounding->position).GetSquaredLength() > queue->midDistSqr) {
										if (object->meshMid) mesh = object->meshMid;
									}

									if (!queue->batchData)
										queue->batchData = new BatchData();
									queue->batchData->addObject(object, mesh);
								}
							}
						}
					} else if (child->type == TYPE_INSTANCE) {
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