/*
 * batch.h
 *
 *  Created on: 2017-4-2
 *      Author: a
 */

#ifndef BATCH_H_
#define BATCH_H_

#include "../mesh/mesh.h"
#include "../render/staticDrawcall.h"

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
	unsigned char* colorBuffer;
	unsigned char* objectidBuffer;
	unsigned int* indexBuffer;

	bool fullStatic;
	unsigned short objectCount;
	float* modelMatrices;
	float* normalMatrices;

	int textureChannel;
	StaticDrawcall* drawcall;

	Batch();
	~Batch();
	void flushBatchBuffers();
	void initBatchBuffers(int vertCount, int indCount);
	void pushMeshToBuffers(Mesh* mesh,int mid,bool fullStatic,const MATRIX4X4& transformMatrix,const MATRIX4X4& normalMatrix);
	void updateMatrices(unsigned short objectId, const MATRIX4X4& transformMatrix, const MATRIX4X4* normalMatrix);
	void setRenderData(int pass, int vertCnt, int indCnt, int objCnt, 
		float* vertices, float* normals, float* texcoords,
		byte* colors, byte* objectids, uint* indices, float* matrices);
	void createDrawcall();
	bool isDynamic();
	void setDynamic(bool dynamic);
};

#endif /* BATCH_H_ */
