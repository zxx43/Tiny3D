#ifndef MESH_GATHER_H_
#define MESH_GATHER_H_

#include "meshManager.h"
#include "meshData.h"
#include "indirectData.h"
#include "meshGroupData.h"
#include "billboardData.h"

class MeshGather {
public:
	MeshData* meshData; // mesh vbo
	MeshData* animData; // anim vbo
	IndirectData* normals;
	IndirectData* singles;
	IndirectData* billbds;
	IndirectData* animats;
	//todo
	MeshGroupData* groupData;
	BillboardData* boardData;
public:
	uint maxSubCount;
private:
	MeshManager* meshManager;
public:
	MeshGather(MeshManager* manager);
	~MeshGather();
public:
	void showLog();
private:
	void addIndirect(SubMesh* sub, IndirectData* indirectData);
};

#endif // !MESH_GATHER_H_

