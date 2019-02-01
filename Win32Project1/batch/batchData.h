#ifndef BATCH_DATA_H_
#define BATCH_DATA_H_

#include "../mesh/mesh.h"
#include "../constants/constants.h"
#include "../object/object.h"

class Batch;

class BatchData {
public:
	float* vertices;
	float* normals;
	float* texcoords;
	byte* colors;
	byte* objectids;
	uint* indices;
	float* matrices;
	int vertexCount, indexCount, objectCount;
	Batch* batch;
public:
	BatchData();
	~BatchData();
	void resetBatch();
	void addObject(Object* object, Mesh* mesh);

};

#endif