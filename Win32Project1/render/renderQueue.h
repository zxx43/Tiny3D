#ifndef RENDER_QUEUE_H_
#define RENDER_QUEUE_H_

#include "render.h"
#include "../node/node.h"
#include <stdlib.h>
#include "../instance/instance.h"
#include "../batch/batch.h"

struct InstanceData {
	Mesh* insMesh;
	float* matrices;
	float* billboards;
	float* positions;
	int count;
	Object* object;
	bool singleSide;
	Instance* instance;
	InstanceData(Mesh* mesh, Object* obj, bool side) {
		insMesh = mesh;
		count = 0;
		matrices = NULL;
		billboards = NULL;
		positions = NULL;
		object = obj;
		singleSide = side;
		instance = NULL;
		if (!mesh->isBillboard) {
			matrices = (float*)malloc(MAX_INSTANCE_COUNT * 12 * sizeof(float));
			memset(matrices, 0, MAX_INSTANCE_COUNT * 12 * sizeof(float));
		} else {
			billboards = (float*)malloc(MAX_INSTANCE_COUNT * 2 * sizeof(float));
			positions = (float*)malloc(MAX_INSTANCE_COUNT * 3 * sizeof(float));
			memset(billboards, 0, MAX_INSTANCE_COUNT * 2 * sizeof(float));
			memset(positions, 0, MAX_INSTANCE_COUNT * 3 * sizeof(float));
		}
	}
	~InstanceData() {
		if (matrices) free(matrices);
		if (billboards) free(billboards);
		if (positions) free(positions);
		if (instance) delete instance;
	}
	void resetInstance() {
		count = 0;
	}
	void addInstance(Object* object) {
		if (matrices)
			memcpy(matrices + (count * 12), object->transformMatrix.GetTranspose().entries, 12 * sizeof(float));
		else {
			memcpy(billboards + (count * 2), object->billboard->data, 2 * sizeof(float));
			memcpy(positions + (count * 3), object->transformMatrix.entries + 12, 3 * sizeof(float));
		}
		count++;
	}
};

class RenderQueue {
private:
	std::vector<Node*> queue;
	std::vector<Batch*> batchQueue;
private:
	void pushDatasToInstance(InstanceData* data);
	void pushObjectToBatch(Object* object, Camera* camera, const VECTOR3D& eye);
public:
	float midDistSqr, lowDistSqr, midDistZ, lowDistZ;
	std::map<Mesh*, InstanceData*> instanceQueue;
public:
	RenderQueue(float midDis, float lowDis);
	~RenderQueue();
	void push(Node* node);
	void flush();
	void deleteInstance(InstanceData* data);
	void draw(Camera* camera,const VECTOR3D& eye,Render* render, RenderState* state);
	void animate(long startTime, long currentTime);
};

void pushNodeToQueue(RenderQueue* queue, Node* node, Camera* camera, const VECTOR3D& eye);

#endif
