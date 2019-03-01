#ifndef INSTANCE_DATA_H_
#define INSTANCE_DATA_H_

#include "../mesh/mesh.h"
#include "../object/object.h"

struct InstanceState {
	bool singleSide;
	bool simple;
	bool grass;
	InstanceState() {
		singleSide = false;
		simple = false;
		grass = false;
	}
	InstanceState(bool sing, bool simp, bool gras) {
		singleSide = sing;
		simple = simp;
		grass = gras;
	}
	InstanceState(const InstanceState& rhs) {
		singleSide = rhs.singleSide;
		simple = rhs.simple;
		grass = rhs.grass;
	}
};

class Instance;

class InstanceData {
public:
	Mesh* insMesh;
	float* matrices;
	float* billboards;
	float* positions;
	int count;
	Object* object;
	InstanceState* state;
	Instance* instance;
public:
	InstanceData(Mesh* mesh, Object* obj, int maxCount, InstanceState* insState);
	~InstanceData();
	void resetInstance();
	void addInstance(Object* object);
	void merge(InstanceData* data);
};

#endif