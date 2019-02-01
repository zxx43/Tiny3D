#ifndef INSTANCE_DATA_H_
#define INSTANCE_DATA_H_

#include "../mesh/mesh.h"
#include "../object/object.h"

class Instance;

class InstanceData {
public:
	Mesh* insMesh;
	float* matrices;
	float* billboards;
	float* positions;
	int count;
	Object* object;
	bool singleSide;
	bool simpleTransform;
	Instance* instance;
public:
	InstanceData(Mesh* mesh, Object* obj, int maxCount, bool side, bool simple);
	~InstanceData();
	void resetInstance();
	void addInstance(Object* object);
	void merge(InstanceData* data);
};

#endif