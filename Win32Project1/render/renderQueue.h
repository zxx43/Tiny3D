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
			billboards = (float*)malloc(MAX_INSTANCE_COUNT * 4 * sizeof(float));
			positions = (float*)malloc(MAX_INSTANCE_COUNT * 3 * sizeof(float));
			memset(billboards, 0, MAX_INSTANCE_COUNT * 4 * sizeof(float));
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
			Material* mat = NULL;
			if (MaterialManager::materials)
				mat = MaterialManager::materials->find(object->billboard->material);

			billboards[count * 4 + 0] = object->billboard->data[0];
			billboards[count * 4 + 1] = object->billboard->data[1];
			if (mat) {
				billboards[count * 4 + 2] = mat->texOfs1.x;
				billboards[count * 4 + 3] = mat->texOfs1.y;
			}

			memcpy(positions + (count * 3), object->transformMatrix.entries + 12, 3 * sizeof(float));
		}
		count++;
	}
};

struct BatchData {
	float* vertices;
	float* normals;
	float* texcoords;
	byte* colors;
	byte* objectids;
	uint* indices;
	float* matrices;
	int vertexCount, indexCount, objectCount;
	Batch* batch;
	BatchData() {
		vertices = (float*)malloc(MAX_VERTEX_COUNT * 3 * sizeof(float));
		normals = (float*)malloc(MAX_VERTEX_COUNT * 3 * sizeof(float));
		texcoords = (float*)malloc(MAX_VERTEX_COUNT * 4 * sizeof(float));
		colors = (byte*)malloc(MAX_VERTEX_COUNT * 3 * sizeof(byte));
		objectids = (byte*)malloc(MAX_VERTEX_COUNT * sizeof(byte));
		indices = (uint*)malloc(MAX_INDEX_COUNT * sizeof(uint));
		matrices = (float*)malloc(MAX_OBJECT_COUNT * 12 * sizeof(float));

		vertexCount = 0;
		indexCount = 0;
		objectCount = 0;
		batch = NULL;
	}
	~BatchData() {
		free(vertices);
		free(normals);
		free(texcoords);
		free(colors);
		free(objectids);
		free(indices);
		free(matrices);
		if (batch) delete batch;
	}
	void resetBatch() {
		vertexCount = 0;
		indexCount = 0;
		objectCount = 0;
	}
	void addObject(Object* object, Mesh* mesh) {
		int baseVertex = vertexCount;
		int currentObject = objectCount++;

		Material* mat = NULL;
		if (object->material >= 0)
			mat = MaterialManager::materials->find(object->material);
		if (!mat) mat = MaterialManager::materials->find(0);

		for (int i = 0; i < mesh->vertexCount; i++) {
			if (mesh->materialids)
				mat = MaterialManager::materials->find(mesh->materialids[i]);

			VECTOR3D vertex3 = mesh->vertices3[i];
			vertices[vertexCount * 3] = vertex3.x;
			vertices[vertexCount * 3 + 1] = vertex3.y;
			vertices[vertexCount * 3 + 2] = vertex3.z;

			normals[vertexCount * 3] = mesh->normals4[i].x;
			normals[vertexCount * 3 + 1] = mesh->normals4[i].y;
			normals[vertexCount * 3 + 2] = mesh->normals4[i].z;

			texcoords[vertexCount * 4] = mesh->texcoords[i].x;
			texcoords[vertexCount * 4 + 1] = mesh->texcoords[i].y;
			texcoords[vertexCount * 4 + 2] = mat->texOfs1.x;
			texcoords[vertexCount * 4 + 3] = mat->texOfs1.y;

			colors[vertexCount * 3] = (byte)(mat->ambient.x * 255);
			colors[vertexCount * 3 + 1] = (byte)(mat->diffuse.x * 255);
			colors[vertexCount * 3 + 2] = (byte)(mat->specular.x * 255);

			objectids[vertexCount++] = currentObject;
		}

		for (int i = 0; i < mesh->indexCount; i++)
			indices[indexCount++] = (uint)(baseVertex + mesh->indices[i]);

		memcpy(matrices + (currentObject * 12), object->transformMatrix.GetTranspose().entries, 12 * sizeof(float));
	}
};

class RenderQueue {
private:
	std::vector<Node*> queue;
private:
	void pushDatasToInstance(InstanceData* data);
	void pushDatasToBatch(BatchData* data, int pass);
public:
	float midDistSqr, lowDistSqr, midDistZ, lowDistZ;
	std::map<Mesh*, InstanceData*> instanceQueue;
	BatchData* batchData;
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
