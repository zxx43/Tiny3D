#ifndef MULTI_INSTANCE_H_
#define MULTI_INSTANCE_H_

#include "instance.h"
#include "../animation/animationData.h"
#include "../constants/constants.h"
#include "../util/util.h"
#include <vector>
#include "../render/multiDrawcall.h"

#define ALL_PASS    0
#define NORMAL_PASS 1
#define SINGLE_PASS 2
#define BILL_PASS   3

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
	int vertexCount, indexCount;
	buff *transformsNormal, *transformsSingle, *transformsBill, *transformsAnim;
	int maxNormalInstance, maxSingleInstance, maxBillInstance, maxAnimInstance;
	int normalInsCount, singleInsCount, billInsCount, animInsCount;
	bool hasAnim;
	int bufferPass;
private:
	std::vector<DataBuffer*> bufferDatas;
	std::vector<DataBuffer*> normalDatas;
	std::vector<DataBuffer*> singleDatas;
	std::vector<DataBuffer*> billDatas;
	std::vector<DataBuffer*> animDatas;
	bool bufferInited;
private:
	std::vector<Indirect*> normals;
	std::vector<Indirect*> singles;
	std::vector<Indirect*> bills;
	std::vector<Indirect*> anims;
private:
	std::vector<Instance*> normalIns;
	std::vector<Instance*> mixedIns;
	std::vector<Instance*> singleIns;
	std::vector<Instance*> billIns;
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
	void add(DataBuffer* dataBuffer);
	void initBuffers(int pass = ALL_PASS);
	int updateTransform();
	void createDrawcall() { drawcall = new MultiDrawcall(this); }
	bool inited() { return bufferInited; }
	Instance* getInstance(int i) { return (Instance*)bufferDatas[i]; }
	Instance* getNormalInstance(int i) { return (Instance*)normalDatas[i]; }
	Instance* getSingleInstance(int i) { return (Instance*)singleDatas[i]; }
	Instance* getBillInstance(int i) { return (Instance*)billDatas[i]; }
	AnimationData* getAnimInstance(int i) { return (AnimationData*)animDatas[i]; }
};

#endif 

