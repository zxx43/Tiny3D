#ifndef MULTI_INSTANCE_H_
#define MULTI_INSTANCE_H_

#include "instance.h"
#include "../constants/constants.h"
#include "../util/util.h"
#include <vector>
#include "../render/multiDrawcall.h"

class MultiInstance {
public:
	float* vertexBuffer;
	half* normalBuffer;
	half* tangentBuffer;
	float* texcoordBuffer;
	float* texidBuffer;
	byte* colorBuffer;
	ushort* indexBuffer;
	buff* transforms;
	int vertexCount, indexCount, instanceCount, maxInstance;
private:
	std::vector<Instance*> insDatas;
	bool bufferInited;
private:
	std::vector<Indirect*> normals;
	std::vector<Indirect*> singles;
public:
	Indirect* indirects;
	uint indirectCount;
	Indirect* indirectsNormal;
	Indirect* indirectsSingle;
	uint normalCount, singleCount;
	uint* normalBases;
	uint* singleBases;
	MultiDrawcall* drawcall;
public:
	MultiInstance();
	~MultiInstance();
	void releaseInstanceData();
	void add(Instance* instance);
	void initBuffers();
	void updateTransform();
	void createDrawcall() { drawcall = new MultiDrawcall(this); }
	bool inited() { return bufferInited; }
};

#endif 

