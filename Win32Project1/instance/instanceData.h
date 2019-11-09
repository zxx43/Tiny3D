#ifndef INSTANCE_DATA_H_
#define INSTANCE_DATA_H_

#include "../mesh/mesh.h"
#include "../object/object.h"

struct InstanceState {
	bool simple, grass;
	InstanceState() : simple(false), grass(false) {}
	InstanceState(bool simp, bool gras) : simple(simp), grass(gras) {}
	InstanceState(const InstanceState& rhs) : simple(rhs.simple), grass(rhs.grass) {}
};

class Instance;

class InstanceData {
public:
	Mesh* insMesh;
	float* matrices;
	float* billboards;
	float* positions;
	float* boundings;
	int count, maxInsCount;
	Object* object;
	InstanceState* state;
	Instance* instance;
public:
	InstanceData(Mesh* mesh, Object* obj, int maxCount, InstanceState* insState);
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