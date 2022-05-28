#ifndef OBJECT_GATHER_H_
#define OBJECT_GATHER_H_

#include "meshManager.h"

class ObjectGather {
public:
	buff* inObjectBuffer; // input object buffer
	uint maxObjectSize; // use to init input object buffer
	uint inObjectCount; // use to update input object buffer
public:
	uint maxNormalSize; // use to init normal submesh output buffer
	uint maxSingleSize; // use to init single submesh output buffer
	uint maxBillbdSize; // use to init billboard submesh output buffer
	uint maxAnimatSize; // use to init animation submesh output buffer
public:
	ObjectGather(const MeshManager* meshManager);
	~ObjectGather();
public:
	void addGroupObject(Object* object);
	void resetGroupObject() { inObjectCount = 0; }
	void showLog();
};

#endif // !OBJECT_GATHER_H_

