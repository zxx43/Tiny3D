#ifndef RENDER_QUEUE_H_
#define RENDER_QUEUE_H_

#include "render.h"
#include "../node/node.h"
#include <stdlib.h>
#include <string.h>
#include "../instance/instance.h"
#include "../batch/batch.h"

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
private:
	void pushDatasToInstance(InstanceData* data, bool copy);
	void pushDatasToBatch(BatchData* data, int pass);
public:
	float midDistSqr, lowDistSqr;
	std::map<Mesh*, InstanceData*> instanceQueue;
	BatchData* batchData;
	bool dual;
	int shadowLevel;
public:
	RenderQueue(float midDis, float lowDis);
	~RenderQueue();
	void push(Node* node);
	void flush();
	void deleteInstance(InstanceData* data);
	void setDual(bool dual);
	void draw(Camera* camera,Render* render, RenderState* state);
	void animate(long startTime, long currentTime);
	Mesh* queryLodMesh(Object* object, const VECTOR3D& eye);
};

void PushNodeToQueue(RenderQueue* queue, Node* node, Camera* camera, Camera* mainCamera);

#endif
