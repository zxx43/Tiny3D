/*
 * batch.h
 *
 *  Created on: 2017-4-2
 *      Author: a
 */

#ifndef BATCH_H_
#define BATCH_H_

#include "../mesh/mesh.h"

#define MAX_OBJECT_COUNT 100

class Batch {
private:
	int storeVertexCount,storeIndexCount;
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

	Batch();
	~Batch();
	void initBatchBuffers(int vertices,int indices);
	void pushMeshToBuffers(Mesh* mesh,int mid,bool fullStatic,const MATRIX4X4& transformMatrix,const MATRIX4X4& normalMatrix);
	void updateMatrices(unsigned short objectId, const MATRIX4X4& transformMatrix, const MATRIX4X4* normalMatrix);
};

#endif /* BATCH_H_ */
