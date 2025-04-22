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
	cfgArgs = cfg;
	shadowLevel = 0;

	objectGather = NULL, processor = NULL;
	debugGather = NULL, debugProcessor = NULL;

	normalDrawcall = NULL, singleDrawcall = NULL, billbdDrawcall = NULL, animatDrawcall = NULL, transpDrawcall = NULL;
	debugDrawcall = NULL;

	forceUpdateInput = false;
	objGatherPrepared = true;
	firstProcess = true;
}

RenderQueue::~RenderQueue() {
	if (processor) delete processor; processor = NULL;
	if (debugProcessor) delete debugProcessor; debugProcessor = NULL;
	if (objectGather) delete objectGather; objectGather = NULL;
	if (debugGather) delete debugGather; debugGather = NULL;

	if (normalDrawcall) delete normalDrawcall; normalDrawcall = NULL;
	if (singleDrawcall) delete singleDrawcall; singleDrawcall = NULL;
	if (billbdDrawcall) delete billbdDrawcall; billbdDrawcall = NULL;
	if (animatDrawcall) delete animatDrawcall; animatDrawcall = NULL;
	if (transpDrawcall) delete transpDrawcall; transpDrawcall = NULL;
	if (debugDrawcall) delete debugDrawcall; debugDrawcall = NULL;
}

void RenderQueue::flush(Scene* scene) {
	if (!objGatherPrepared) return;
	if (staticDataReady()) return;

	if (objectGather) objectGather->resetGroupObject();
	if (debugGather) debugGather->resetGroupObject();
}

void RenderQueue::process(Scene* scene, Render* render, const RenderState* state, const LodParam& param) {
	if (!objGatherPrepared) return;
	doProcess(scene, render, state, param);
	if (firstProcess) {
		doProcess(scene, render, state, param);
		firstProcess = false;
	}
}

void RenderQueue::doProcess(Scene* scene, Render* render, const RenderState* state, const LodParam& param) {
	if (!isDebugQueue()) {
		if (!scene->meshGather) scene->createMeshGather();
		if (!scene->meshBuffer && scene->meshGather) scene->createMeshBuffer();
		if (!objectGather) {
			if (scene->meshGather && scene->meshBuffer) objectGather = new ObjectGather(scene->meshMgr);
		}
		if (!processor) {
			if (objectGather) {
				processor = new Processor(scene->meshGather, scene->meshBuffer, objectGather);
			}
		} else {
			if (!staticDataReady() || forceUpdateInput) {
				processor->update();
				if (forceUpdateInput) finishForceUpdate();
			}
			processor->clear(render);
			processor->lod(render, state, param);
			processor->rearrange(render);
			processor->gather(render);
		}
	} else {
		if (!scene->debugMeshGather) scene->createDebugGather();
		if (!scene->debugMeshBuffer && scene->debugMeshGather) scene->createDebugBuffer();
		if (!debugGather) {
			if (scene->debugMeshGather && scene->debugMeshBuffer) debugGather = new ObjectGather(scene->debugMeshMgr);
		}
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
	if (!objGatherPrepared) return;
	if (isDebugQueue()) {
		if (!debugDrawcall && scene->debugMeshBuffer && debugProcessor && debugProcessor->indNormalCount > 0) debugDrawcall = new IndirectDrawcall(debugProcessor, scene->debugMeshBuffer, INDIRECT_NORMAL);
	} else {
		if (!normalDrawcall && scene->meshBuffer && processor && processor->indNormalCount > 0) normalDrawcall = new IndirectDrawcall(processor, scene->meshBuffer, INDIRECT_NORMAL);
		if (!singleDrawcall && scene->meshBuffer && processor && processor->indSingleCount > 0) singleDrawcall = new IndirectDrawcall(processor, scene->meshBuffer, INDIRECT_SINGLE);
		if (!billbdDrawcall && scene->meshBuffer && processor && processor->indBillbdCount > 0) billbdDrawcall = new IndirectDrawcall(processor, scene->meshBuffer, INDIRECT_BILLBD);
		if (!animatDrawcall && scene->meshBuffer && processor && processor->indAnimatCount > 0) animatDrawcall = new IndirectDrawcall(processor, scene->meshBuffer, INDIRECT_ANIMAT);
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

void RenderQueue::drawTransparent(Scene* scene, Camera* camera, Render* render, RenderState* state) {
	if (!objGatherPrepared) return;
	if (!isDebugQueue()) {
		if (!transpDrawcall && scene->meshBuffer && processor && processor->indTranspCount > 0)
			transpDrawcall = new IndirectDrawcall(processor, scene->meshBuffer, INDIRECT_TRANSP);
	}
	if (processor && processor->inputObjectCount > 0)
		if (transpDrawcall) render->draw(camera, transpDrawcall, state);
}

void RenderQueue::clearRenderData() {
	if (normalDrawcall) delete normalDrawcall; normalDrawcall = NULL;
	if (singleDrawcall) delete singleDrawcall; singleDrawcall = NULL;
	if (billbdDrawcall) delete billbdDrawcall; billbdDrawcall = NULL;
	if (animatDrawcall) delete animatDrawcall; animatDrawcall = NULL;
	if (transpDrawcall) delete transpDrawcall; transpDrawcall = NULL;
	if (processor) delete processor; processor = NULL;

	if (debugDrawcall) delete debugDrawcall; debugDrawcall = NULL;
	if (debugProcessor) delete debugProcessor; debugProcessor = NULL;

	if (objectGather) delete objectGather; objectGather = NULL;
	if (debugGather) delete debugGather; debugGather = NULL;
	needResetObjGather();
	firstProcess = true;
}

bool RenderQueue::staticDataReady() { 
	if (processor) return isStaticQueue() && processor->isInputPushed();
	return false; 
}

void PushDebugToQueue(RenderQueue* queue, Scene* scene, Camera* camera) {
	if (queue->debugGather && queue->isDebugQueue()) {
		for (uint i = 0; i < scene->boundingNodes.size(); ++i) {
			Node* node = scene->boundingNodes[i];
			if (node->checkInCamera(camera) && node->objects.size() > 0) {
				Object* object = node->objects[0];
				if (object->isDebug() && queue->debugGather) queue->debugGather->addGroupObject(object);
			}
		}
	}
}

void PushNodeToQueue(RenderQueue* queue, Scene* scene, Node* node, Camera* camera, Camera* mainCamera) {
	if (!queue->objectGather || queue->isStaticQueue()) return;
	if (node->checkInCamera(camera)) {
		for (uint i = 0; i<node->children.size(); ++i) {
			Node* child = node->children[i];
			if (child->objects.size() <= 0)
				PushNodeToQueue(queue, scene, child, camera, mainCamera);
			else {
				if (child->shadowLevel < queue->shadowLevel) continue;
				else if (child->checkInCamera(camera)) {
					if (child->type == TYPE_INSTANCE) {
						for (uint j = 0; j < child->dynamicObjects.size(); ++j) {
							Object* object = child->dynamicObjects[j];

							if (queue->shadowLevel > 0 && !object->genShadow) continue;
							if (object->isDebug()) continue;
							if (object->sphereInCamera(camera) && queue->objectGather) {
								queue->objectGather->addGroupObject(object);
							}
						}
					} else if (child->type == TYPE_ANIMATE) {
						AnimationNode* animNode = (AnimationNode*)child;
						Object* object = animNode->getObject();
						if (object && !object->isDebug() && queue->objectGather) {
							queue->objectGather->addGroupObject(object);
							animNode->animate(scene->velocity);
						}
					} 
				}
			}
		}
	}
}

void ResetStaticQueueData(RenderQueue* queue, Scene* scene, Node* node) {
	if (!queue->isStaticQueue()) return;
	if (queue->objectGather) queue->objectGather->resetGroupObject();
	InitNodeToQueue(queue, scene, node);
	queue->forceUpdateData();
}

void InitNodeToQueue(RenderQueue* queue, Scene* scene, Node* node) {
	if (!queue->objectGather || !queue->isStaticQueue()) return;
	for (uint i = 0; i < node->children.size(); ++i) {
		Node* child = node->children[i];
		if (child->objects.size() <= 0)
			InitNodeToQueue(queue, scene, child);
		else {
			if (child->type == TYPE_INSTANCE) {
				for (uint j = 0; j < child->staticObjects.size(); ++j) {
					Object* object = child->staticObjects[j];
					if (!object->isDebug() && queue->objectGather) queue->objectGather->addGroupObject(object);
				}
			}
		}
	}
}