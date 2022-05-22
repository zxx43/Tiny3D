#ifndef MESH_GROUP_DATA_H_
#define MESH_GROUP_DATA_H_

#include "../constants/constants.h"
#include "../util/util.h"
#include "meshManager.h"

class MeshGroupData {
public:
	int* buffer;
	int count;
	int channel;
public:
	MeshGroupData();
	~MeshGroupData();
public:
	void append(const MeshGroup* meshGroup);
private:
	void releaseBuffer();
};

#endif // !MESH_GROUP_DATA_H_

