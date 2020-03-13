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
#define QUEUE_STATIC_SN 0
#define QUEUE_STATIC_SM 1
#define QUEUE_STATIC_SF 2
#define QUEUE_ANIMATE_SN 3
#define QUEUE_ANIMATE_SM 4
#define QUEUE_ANIMATE_SF 5
#define QUEUE_STATIC 6
#define QUEUE_ANIMATE 7
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
	int queueType;
	float midDistSqr, lowDistSqr;
	std::map<Mesh*, InstanceData*> instanceQueue;
	std::map<Animation*, AnimationData*> animationQueue;
	MultiInstance* multiInstance;
	MultiInstance* billboards;
	MultiInstance* animations;
	bool dualInstances;
	BatchData* batchData;
	bool dual;
	int shadowLevel;
	bool firstFlush;
public:
	RenderQueue(int type, float midDis, float lowDis);
	~RenderQueue();
	void push(Node* node);
	void pushAnim(Node* node);
	void flush();
	void deleteInstance(InstanceData* data);
	void setDual(bool dual);
	void draw(Scene* scene, Camera* camera, Render* render, RenderState* state);
	void animate(float velocity);
	Mesh* queryLodMesh(Object* object, const vec3& eye);
};

void PushNodeToQueue(RenderQueue* queue, Scene* scene, Node* node, Camera* camera, Camera* mainCamera);

#endif
