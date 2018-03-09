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
#include "../render/instanceDrawcall.h"

#define MAX_INSTANCE_COUNT 4096

class Instance {
public:
	Mesh* instanceMesh;
	int vertexCount,indexCount;
	float* vertexBuffer;
	float* normalBuffer;
	float* texcoordBuffer;
	unsigned char* colorBuffer;
	unsigned short* indexBuffer;

	int textureChannel;
	int instanceCount;
	float* modelMatrices;
	InstanceDrawcall* drawcall;
	bool singleSide;

	Instance(Mesh* mesh);
	~Instance();
	void initInstanceBuffers(int mid,int vertices,int indices);
	void setInstanceCount(int count);
	void updateMatricesBuffer(int i, const MATRIX4X4& transformMatrix);
	void createDrawcall(bool simple);
private:
	void initMatrices();
};

#endif /* INSTANCE_H_ */
