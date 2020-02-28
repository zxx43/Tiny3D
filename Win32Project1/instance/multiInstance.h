#ifndef MULTI_INSTANCE_H_
#define MULTI_INSTANCE_H_

#include "instance.h"
#include "../animation/animationData.h"
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
	byte* boneidBuffer;
	half* weightBuffer;
	ushort* indexBuffer;
	buff* transforms;
	int vertexCount, indexCount, instanceCount, maxInstance;
	bool hasAnim;
private:
	std::vector<Instance*> insDatas;
	std::vector<AnimationData*> animDatas;
	bool bufferInited;
private:
	std::vector<Indirect*> normals;
	std::vector<Indirect*> singles;
	std::vector<Indirect*> bills;
	std::vector<Indirect*> anims;
public:
	Indirect* indirects;
	uint indirectCount;
	Indirect* indirectsNormal;
	Indirect* indirectsSingle;
	Indirect* indirectsBill;
	Indirect* indirectsAnim;
	uint normalCount, singleCount, billCount, animCount, meshCount;
	uint* bases;
	MultiDrawcall* drawcall;
public:
	MultiInstance();
	~MultiInstance();
	void releaseInstanceData();
	void add(Instance* instance);
	void add(AnimationData* animData);
	void initBuffers();
	void updateTransform();
	void createDrawcall() { drawcall = new MultiDrawcall(this); }
	bool inited() { return bufferInited; }
};

#endif 

