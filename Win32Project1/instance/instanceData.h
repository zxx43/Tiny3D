#ifndef INSTANCE_DATA_H_
#define INSTANCE_DATA_H_

#include "../mesh/mesh.h"
#include "../object/object.h"

class Instance;

class InstanceData {
public:
	Mesh* insMesh;
	float* matrices;
	buff* transformsFull;
	bill* billboards;
	int count, maxInsCount;
	Object* object;
	Instance* instance;
public:
	InstanceData(Mesh* mesh, Object* obj, int maxCount);
	~InstanceData();
	void resetInstance();
	void addInstance(Object* object);
	void merge(InstanceData* data);
private:
	void doMergeData(InstanceData* data);
private:
	std::vector<InstanceData*> subGroup;
	int groupToMerge;
};

#endif