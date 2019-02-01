/*
 * instance.h
 *
 *  Created on: 2017-9-28
 *      Author: a
 */

#ifndef INSTANCE_H_
#define INSTANCE_H_

#include "../render/instanceDrawcall.h"
#include "instanceData.h"

#define MAX_INSTANCE_COUNT 819200

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

	int instanceCount;
	float* modelMatrices;
	float* positions;
	float* billboards;
	InstanceDrawcall* drawcall;
	bool singleSide;
	bool isBillboard;
	bool isDynamic;
	bool isSimple;
	bool copyData;

	Instance(Mesh* mesh, bool dyn, bool simp);
	~Instance();
	void releaseInstanceData();
	void initInstanceBuffers(Object* object,int vertices,int indices,int cnt,bool copy);
	void setRenderData(InstanceData* data);
	void addObject(Object* object, int index);
	void createDrawcall();
private:
	void initMatrices(int cnt);
	void initBillboards(int cnt);
};

#endif /* INSTANCE_H_ */
