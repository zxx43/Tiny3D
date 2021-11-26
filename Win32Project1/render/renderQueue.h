#ifndef RENDER_QUEUE_H_
#define RENDER_QUEUE_H_

#include "render.h"
#include "../node/node.h"
#include <stdlib.h>
#include <string.h>
#include "../instance/instance.h"
#include "../instance/multiInstance.h"
#include "../batch/batch.h"
#include "../animation/animationData.h"

#ifndef QUEUE_STATIC
#define QUEUE_SIZE       9
#define QUEUE_DYNAMIC_SN 0
#define QUEUE_STATIC_SN  1
#define QUEUE_STATIC_SM  2
#define QUEUE_STATIC_SF  3
#define QUEUE_ANIMATE_SN 4
#define QUEUE_ANIMATE_SM 5
#define QUEUE_ANIMATE_SF 6
#define QUEUE_STATIC     7
#define QUEUE_ANIMATE    8
#define QUEUE_DEBUG      9
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
private:
	Queue* queue;
	Queue* animQueue;
private:
	void pushDatasToInstance(Scene* scene, InstanceData* data, bool copy);
	void pushDatasToBatch(BatchData* data, int pass);
public:
	ConfigArg* cfgArgs;
	int queueType;
	float midDistSqr, lowDistSqr;
	std::map<Mesh*, InstanceData*> instanceQueue;
	std::map<Animation*, AnimationData*> animationQueue;
	InstanceData* instanceDebug;
	MultiInstance* multiInstance;
	MultiInstance* singleInstance;
	MultiInstance* billboards;
	MultiInstance* animations;
	MultiInstance* boundings;
	BatchData* batchData;
	int shadowLevel;
	bool firstFlush;
public:
	RenderQueue(int type, float midDis, float lowDis, ConfigArg* cfg);
	~RenderQueue();
	void push(Node* node);
	void pushAnim(Node* node);
	void flush();
	void deleteInstance(InstanceData* data);
	void createInstances(Scene* scene);
	void draw(Scene* scene, Camera* camera, Render* render, RenderState* state);
	void animate(float velocity);
	Mesh* queryLodMesh(Object* object, const vec3& eye);
};

void PushNodeToQueue(RenderQueue* queue, Scene* scene, Node* node, Camera* camera, Camera* mainCamera);
void PushDebugToQueue(RenderQueue* queue, Scene* scene, Camera* camera);

#endif
