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
#include "../object/object.h"

#define MAX_INSTANCE_COUNT 8192

class Instance {
public:
	static std::map<Mesh*, int> instanceTable;
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
	float* positions;
	float* billboards;
	InstanceDrawcall* drawcall;
	bool singleSide;
	bool isBillboard;

	Instance(Mesh* mesh);
	~Instance();
	void initInstanceBuffers(Object* object,int vertices,int indices);
	void setInstanceCount(int count);
	void setRenderData(float* matrices, float* billboards, float* positions);
	void createDrawcall();
private:
	void initMatrices();
	void initBillboards();
};

#endif /* INSTANCE_H_ */
