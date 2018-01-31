/*
 * batch.h
 *
 *  Created on: 2017-4-2
 *      Author: a
 */

#ifndef BATCH_H_
#define BATCH_H_

#include "../mesh/mesh.h"

class Batch {
private:
	int storeVertexCount,storeIndexCount;
	void initMatrix(const MATRIX4X4& transformMatrix,const MATRIX4X4& normalMatrix);
public:
	int vertexCount,indexCount;
	float* vertexBuffer;
	float* normalBuffer;
	float* texcoordBuffer;
	unsigned char* colorBuffer;
	float* modelMatrices;
	float* normalMatrices;
	unsigned int* indexBuffer;

	bool fullStatic;
	int textureChannel;

	Batch();
	~Batch();
	void initBatchBuffers(int vertices,int indices);
	void pushMeshToBuffers(Mesh* mesh,int mid,bool fullStatic,const MATRIX4X4& transformMatrix,const MATRIX4X4& normalMatrix);
	void updateMatricesBuffer(int baseVertex, Mesh* mesh, const MATRIX4X4& transformMatrix, const MATRIX4X4* normalMatrix);
};

#endif /* BATCH_H_ */
