/*
 * instance.h
 *
 *  Created on: 2017-9-28
 *      Author: a
 */

#ifndef INSTANCE_H_
#define INSTANCE_H_

#include "instanceData.h"
#include "../render/dataBuffer.h"

class Instance: public DataBuffer {
public:
	static std::map<Mesh*, int> instanceTable;
	InstanceData* insData;
public:
	Mesh* instanceMesh;
	int insId, insSingleId, insBillId;
	bool isBillboard, hasNormal, hasSingle;
public:
	Instance(InstanceData* data);
	Instance(Mesh* mesh);
	virtual ~Instance();
public:
	virtual void releaseDatas();
	void initInstanceBuffers(int vertices,int indices,int cnt,bool copy);
	void setRenderData(InstanceData* data);
private:
	void create(Mesh* mesh);
};

#endif /* INSTANCE_H_ */
