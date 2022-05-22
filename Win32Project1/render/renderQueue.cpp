#include "renderQueue.h"
#include "../node/staticNode.h"
#include "../node/animationNode.h"
#include "../node/instanceNode.h"
#include "../assets/assetManager.h"
#include "../scene/scene.h"
#include "../gather/meshBuffer.h"
#include <string.h>
#include <stdlib.h>
using namespace std;

RenderQueue::RenderQueue(int type, float midDis, float lowDis, ConfigArg* cfg) {
	queueType = type;
	queue = new Queue(1);
	animQueue = new Queue(1);
	batchData = NULL;
	midDistSqr = powf(midDis, 2);
	lowDistSqr = powf(lowDis, 2);
	shadowLevel = 0;
	cfgArgs = cfg;

	objectGather = NULL, processor = NULL;
	debugGather = NULL, debugProcessor = NULL;

	normalDrawcall = NULL, singleDrawcall = NULL, billbdDrawcall = NULL, animatDrawcall = NULL;
	debugDrawcall = NULL;
}

RenderQueue::~RenderQueue() {
	delete queue;
	delete animQueue;

	if (processor) delete processor; processor = NULL;
	if (debugProcessor) delete debugProcessor; debugProcessor = NULL;
	if (objectGather) delete objectGather; objectGather = NULL;
	if (debugGather) delete debugGather; debugGather = NULL;

	if (normalDrawcall) delete normalDrawcall; normalDrawcall = NULL;
	if (singleDrawcall) delete singleDrawcall; singleDrawcall = NULL;
	if (billbdDrawcall) delete billbdDrawcall; billbdDrawcall = NULL;
	if (animatDrawcall) delete animatDrawcall; animatDrawcall = NULL;
	if (debugDrawcall) delete debugDrawcall; debugDrawcall = NULL;

	if (batchData) delete batchData;
}

void RenderQueue::push(Node* node) {
	queue->push(node);
}

void RenderQueue::pushAnim(Node* node) {
	animQueue->push(node);
}

void RenderQueue::flush(Scene* scene) {
	queue->flush();
	animQueue->flush();

	if (!isDebugQueue()) {
		if (objectGather) objectGather->resetGroupObject();
		else {
			if (scene->meshGather && scene->meshBuffer) {
				objectGather = new ObjectGather(scene->meshMgr, isAnimQueue());
				objectGather->showLog();
			}
		}
	} else {
		if (debugGather) debugGather->resetGroupObject();
		else {
			if (scene->debugMeshGather && scene->debugMeshBuffer) {
				debugGather = new ObjectGather(scene->debugMeshMgr, false);
				debugGather->showLog();
			}
		}
	}

	if (batchData) batchData->resetBatch();
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

void RenderQueue::process(Scene* scene, Render* render, const RenderState* state, const LodParam& param) {
	if (!isDebugQueue()) {
		if (!scene->meshBuffer && scene->meshGather) scene->createMeshBuffer();
		if (!processor) {
			if (objectGather) {
				processor = new Processor(scene->meshGather, scene->meshBuffer, objectGather);
			}
		} else {
			processor->update();
			processor->clear(render);
			processor->lod(render, state, param);
			processor->rearrange(render);
			processor->gather(render);
		}
	} else {
		if (!scene->debugMeshBuffer && scene->debugMeshGather) scene->createDebugBuffer();
		if (!debugProcessor) {
			if (debugGather) {
				debugProcessor = new Processor(scene->debugMeshGather, scene->debugMeshBuffer, debugGather);
			}
		} else {
			debugProcessor->update();
			debugProcessor->clear(render);
			debugProcessor->lod(render, state, param);
			debugProcessor->rearrange(render);
			debugProcessor->gather(render);
		}
	}
}

void RenderQueue::draw(Scene* scene, Camera* camera, Render* render, RenderState* state) {
	for (int it = 0; it < queue->size; it++) {
		Node* node = queue->get(it);

		if (node->type == TYPE_STATIC) 
			render->draw(camera, node->drawcall, state);
		else if (node->type == TYPE_TERRAIN) {
			if (state->pass == COLOR_PASS) {
				static Shader* terrainShader = render->findShader("terrain");
				Shader* shader = state->shader;
				state->shader = terrainShader;
				render->draw(camera, node->drawcall, state);
				state->shader = shader;
			}
		}
	}

	if (batchData) {
		pushDatasToBatch(batchData, state->pass);
		Batch* batch = batchData->batch;
		if (batch && !batch->hasTerrain && !batch->hasWater) {
			if (!batch->drawcall) batch->createDrawcall();
			if (batch->objectCount > 0) {
				batch->drawcall->updateBuffers(state->pass);
				batch->drawcall->updateMatrices();
				render->draw(camera, batch->drawcall, state);
			}
		}
	}

	if (isDebugQueue()) {
		if (!debugDrawcall && scene->debugMeshBuffer && debugProcessor && debugProcessor->indNormalCount) debugDrawcall = new IndirectDrawcall(debugProcessor, scene->debugMeshBuffer, INDIRECT_NORMAL);
	} else {
		if (isAnimQueue()) {
			if (!animatDrawcall && scene->meshBuffer && processor && processor->indAnimatCount) animatDrawcall = new IndirectDrawcall(processor, scene->meshBuffer, INDIRECT_ANIMAT);
		} else {
			if (!normalDrawcall && scene->meshBuffer && processor && processor->indNormalCount) normalDrawcall = new IndirectDrawcall(processor, scene->meshBuffer, INDIRECT_NORMAL);
			if (!singleDrawcall && scene->meshBuffer && processor && processor->indSingleCount) singleDrawcall = new IndirectDrawcall(processor, scene->meshBuffer, INDIRECT_SINGLE);
			if (!billbdDrawcall && scene->meshBuffer && processor && processor->indBillbdCount) billbdDrawcall = new IndirectDrawcall(processor, scene->meshBuffer, INDIRECT_BILLBD);
		}
	}

	if (processor && processor->inputObjectCount > 0) {
		if (normalDrawcall) render->draw(camera, normalDrawcall, state);
		if (singleDrawcall) render->draw(camera, singleDrawcall, state);
		if (billbdDrawcall) render->draw(camera, billbdDrawcall, state);
		if (animatDrawcall) render->draw(camera, animatDrawcall, state);
	}

	if (debugProcessor && debugProcessor->inputObjectCount > 0) {
		if (debugDrawcall) render->draw(camera, debugDrawcall, state);
	}
}

void RenderQueue::animate(float velocity) {
	for (int it = 0; it < animQueue->size; it++) {
		AnimationNode* animateNode = (AnimationNode*)animQueue->get(it);
		animateNode->animate(velocity);
	}
}

void PushDebugToQueue(RenderQueue* queue, Scene* scene, Camera* camera) {
	if (queue->debugGather) {
		for (uint i = 0; i < scene->boundingNodes.size(); ++i) {
			Node* node = scene->boundingNodes[i];
			if (node->checkInCamera(camera) && node->objects.size() > 0) {
				Object* object = node->objects[0];
				if (object->isDebug()) queue->debugGather->addGroupObject(object);
			}
		}
	}
}

void PushNodeToQueue(RenderQueue* queue, Scene* scene, Node* node, Camera* camera, Camera* mainCamera) {
	if (node->checkInCamera(camera)) {
		for (unsigned int i = 0; i<node->children.size(); ++i) {
			Node* child = node->children[i];
			if (child->objects.size() <= 0)
				PushNodeToQueue(queue, scene, child, camera, mainCamera);
			else {
				if (child->shadowLevel < queue->shadowLevel) continue;

				if (child->checkInCamera(camera)) {
					if (child->type != TYPE_INSTANCE && child->type != TYPE_STATIC && child->type != TYPE_ANIMATE)
						queue->push(child);
					else if (child->type == TYPE_INSTANCE) {
						for (uint j = 0; j < child->objects.size(); ++j) {
							Object* object = child->objects[j];
							if (queue->queueType == QUEUE_DYNAMIC_SN && !object->isDynamic()) continue;
							else if (queue->queueType == QUEUE_STATIC_SN && object->isDynamic()) continue;

							if (queue->shadowLevel > 0 && !object->genShadow) continue;
							if (object->sphereInCamera(camera)) {
								if (!object->isDebug() && queue->objectGather) queue->objectGather->addGroupObject(object);
							}
						}
					} else if (child->type == TYPE_ANIMATE) {
						if (child->objects.size() > 0) {
							queue->pushAnim(child);
							AnimationNode* animNode = (AnimationNode*)child;
							animNode->animate(scene->velocity);

							Object* object = animNode->getObject();
							if (!object->isDebug() && queue->objectGather) queue->objectGather->addGroupObject(object);
						}
					} 
				}
			}
		}
	}
}