#ifndef RENDER_QUEUE_H_
#define RENDER_QUEUE_H_

#include "render.h"
#include "../node/node.h"
#include <stdlib.h>
#include <string.h>
#include "../batch/batch.h"
#include "../render/indirectDrawcall.h"

#ifndef QUEUE_STATIC
#define QUEUE_SIZE      6
#define QUEUE_STATIC    0
#define QUEUE_DYN_SNEAR 1
#define QUEUE_DYN_SMID  2
#define QUEUE_DYN_SFAR  3
#define QUEUE_DYN_MAIN  4
#define QUEUE_DEBUG     5
#endif

struct Queue {
	Node** data;
	int capacity, size;
	Queue(int count) {
		capacity = count;
		data = (Node**)malloc(capacity*sizeof(Node*));
		size = 0;
	}
	~Queue() {
		free(data); data = NULL;
	}
	void push(Node* node) {
		size++;
		if (size > capacity) {
			int capacityBefore = capacity;
			capacity += 10;
			Node** tmp = (Node**)malloc(capacity*sizeof(Node*));
			memcpy(tmp, data, capacityBefore * sizeof(Node*));
			free(data);
			data = tmp;
		}
		*(data + size - 1) = node;
	}
	void flush() {
		size = 0;
	}
	Node* get(int i) {
		if (i < size)
			return *(data + i);
		return NULL;
	}
};

class RenderQueue {
public:
	ObjectGather* objectGather;
	Processor* processor;
	ObjectGather* debugGather;
	Processor* debugProcessor;
private:
	IndirectDrawcall* normalDrawcall;
	IndirectDrawcall* singleDrawcall;
	IndirectDrawcall* billbdDrawcall;
	IndirectDrawcall* animatDrawcall;
	IndirectDrawcall* transpDrawcall;
	IndirectDrawcall* debugDrawcall;
private:
	bool forceUpdateInput;
	bool objGatherPrepared;
public:
	ConfigArg* cfgArgs;
	int queueType;
	int shadowLevel;
public:
	RenderQueue(int type, float midDis, float lowDis, ConfigArg* cfg);
	~RenderQueue();
public:
	void flush(Scene* scene);
	void process(Scene* scene, Render* render, const RenderState* state, const LodParam& param);
	void draw(Scene* scene, Camera* camera, Render* render, RenderState* state);
	void drawTransparent(Scene* scene, Camera* camera, Render* render, RenderState* state);
	void clearRenderData();
	bool isDebugQueue() { return queueType == QUEUE_DEBUG; }
	bool isStaticQueue() { return queueType == QUEUE_STATIC; }
	void forceUpdateData() { forceUpdateInput = true; }
	void finishForceUpdate() { forceUpdateInput = false; }
	bool staticDataReady();
	void needResetObjGather() { objGatherPrepared = false; }
	void resetObjGatherFin() { objGatherPrepared = true; }
	bool isObjGatherPrepared() { return objGatherPrepared; }
private:
	bool firstProcess;
private:
	void doProcess(Scene* scene, Render* render, const RenderState* state, const LodParam& param);
};

void ResetStaticQueueData(RenderQueue* queue, Scene* scene, Node* node);
void InitNodeToQueue(RenderQueue* queue, Scene* scene, Node* node);
void PushNodeToQueue(RenderQueue* queue, Scene* scene, Node* node, Camera* camera, Camera* mainCamera);
void PushDebugToQueue(RenderQueue* queue, Scene* scene, Camera* camera);

#endif
