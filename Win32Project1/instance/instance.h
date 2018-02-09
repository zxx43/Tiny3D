/*
 * instance.h
 *
 *  Created on: 2017-9-28
 *      Author: a
 */

#ifndef INSTANCE_H_
#define INSTANCE_H_

#include "../mesh/mesh.h"
#include "../maths/MATRIX4X4.h"
#include <vector>

class Instance {
private:
	std::vector<MATRIX4X4> modelMatrixList,normalMatrixList;
public:
	Mesh* instanceMesh;
	int vertexCount,indexCount;
	float* vertexBuffer;
	float* normalBuffer;
	float* texcoordBuffer;
	unsigned char* colorBuffer;
	unsigned short* indexBuffer;

	int instanceCount;
	int textureChannel;
	float* modelMatrices;
	float* normalMatrices;

	Instance(Mesh* mesh);
	~Instance();
	void initInstanceBuffers(int mid,int vertices,int indices);
	void pushObjectToInstances(const MATRIX4X4& transformMatrix,const MATRIX4X4& normalMatrix);
	void updateMatricesBuffer(int baseInstance, const MATRIX4X4& transformMatrix, const MATRIX4X4* normalMatrix);
	void initMatrices();
};

#endif /* INSTANCE_H_ */
