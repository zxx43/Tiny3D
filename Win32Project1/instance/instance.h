/*
 * instance.h
 *
 *  Created on: 2017-9-28
 *      Author: a
 */

#ifndef INSTANCE_H_
#define INSTANCE_H_

#include "instanceData.h"

class Instance {
public:
	static std::map<Mesh*, int> instanceTable;
public:
	int insId, insSingleId, insBillId;
	Mesh* instanceMesh;
	int vertexCount,indexCount;
	float* vertexBuffer;
	half* normalBuffer;
	half* tangentBuffer;
	float* texcoordBuffer;
	float* texidBuffer;
	unsigned char* colorBuffer;
	unsigned short* indexBuffer;

	int maxInstanceCount;

	bool isBillboard, hasNormal, hasSingle;

	InstanceData* insData;

	Instance(InstanceData* data);
	Instance(Mesh* mesh);
	~Instance();
	void releaseInstanceData();
	void initInstanceBuffers(Object* object,int vertices,int indices,int cnt,bool copy);
	void setRenderData(InstanceData* data);
private:
	void create(Mesh* mesh);
};

#endif /* INSTANCE_H_ */
