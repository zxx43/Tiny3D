/*
 * batch.h
 *
 *  Created on: 2017-4-2
 *      Author: a
 */

#ifndef BATCH_H_
#define BATCH_H_

#include "../render/staticDrawcall.h"
#include "batchData.h"

#ifndef MAX_OBJECT_COUNT
#define MAX_OBJECT_COUNT 100
#define MAX_VERTEX_COUNT 131072
#define MAX_INDEX_COUNT 131072
#define BATCH_TYPE_DYNAMIC 0
#define BATCH_TYPE_STATIC 1
#endif

class Batch {
private:
	uint type;
private:
	void initMatrix(unsigned short currentObject,const MATRIX4X4& transformMatrix,const MATRIX4X4& normalMatrix);
public:
	int vertexCount,indexCount;
	float* vertexBuffer;
	float* normalBuffer;
	float* texcoordBuffer;
	float* texOfsBuffer;
	unsigned char* colorBuffer;
	unsigned char* objectidBuffer;
	unsigned int* indexBuffer;

	bool fullStatic;
	unsigned short objectCount;
	float* modelMatrices;
	float* normalMatrices;
	float* matrixDataPtr;

	int textureCount;
	StaticDrawcall* drawcall;

	Batch();
	~Batch();
	void releaseBatchData();
	void flushBatchBuffers();
	void initBatchBuffers(int vertCount, int indCount);
	void pushMeshToBuffers(Mesh* mesh,int mid,bool fullStatic,const MATRIX4X4& transformMatrix,const MATRIX4X4& normalMatrix);
	void updateMatrices(unsigned short objectId, const MATRIX4X4& transformMatrix, const MATRIX4X4* normalMatrix);
	void setRenderData(int pass, BatchData* data);
	void createDrawcall();
	bool isDynamic();
	void setDynamic(bool dynamic);
};

#endif /* BATCH_H_ */
