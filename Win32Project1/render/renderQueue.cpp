#include "renderQueue.h"
#include "../node/animationNode.h"

RenderQueue::RenderQueue() {
	queue.clear();
}

RenderQueue::~RenderQueue() {
	queue.clear();
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

void RenderQueue::draw(Camera* camera, Render* render, RenderState* state) {
	for (unsigned int i = 0; i < queue.size(); i++) {
		Node* node = queue[i];
		if (!node->drawcall || node->needCreateDrawcall) 
			node->prepareDrawcall();
		else if (node->needUpdateDrawcall) 
			node->updateDrawcall(node->needUpdateNormal);

		if (node->uTransformMatrix) {
			if (!node->drawcall->uModelMatrix)
				node->drawcall->uModelMatrix = new float[16];
			for (int m = 0; m < 4; m++) {
				node->drawcall->uModelMatrix[m] = node->uTransformMatrix->entries[m];
				node->drawcall->uModelMatrix[m + 4] = node->uTransformMatrix->entries[m + 4];
				node->drawcall->uModelMatrix[m + 8] = node->uTransformMatrix->entries[m + 8];
				node->drawcall->uModelMatrix[m + 12] = node->uTransformMatrix->entries[m + 12];
			}
		}
		if (node->uNormalMatrix) {
			if (!node->drawcall->uNormalMatrix)
				node->drawcall->uNormalMatrix = new float[9];
			for (int m = 0; m < 3; m++) {
				node->drawcall->uNormalMatrix[m] = node->uNormalMatrix->entries[m];
				node->drawcall->uNormalMatrix[m + 3] = node->uNormalMatrix->entries[m + 4];
				node->drawcall->uNormalMatrix[m + 6] = node->uNormalMatrix->entries[m + 8];
			}
		}

		render->draw(camera, node->drawcall, state);
	}
}

void RenderQueue::animate(long startTime, long currentTime) {
	for (unsigned int i = 0; i < queue.size(); i++) {
		Node* node = queue[i];
		AnimationNode* animateNode = dynamic_cast<AnimationNode*>(node);
		if (animateNode)
			animateNode->animate(0, startTime, currentTime);
	}
}

void pushNodeToQueue(RenderQueue* queue, Node* node, Camera* camera) {
	if (node->checkInCamera(camera)) {
		for (unsigned int i = 0; i<node->children.size(); i++) {
			Node* child = node->children[i];
			if (child->objects.size()>0) {
				if (child->checkInCamera(camera))
					queue->push(child);
			}
			else
				pushNodeToQueue(queue, child, camera);
		}
	}
}
