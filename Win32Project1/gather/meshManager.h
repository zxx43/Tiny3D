#ifndef MESH_MANAGER_H_
#define MESH_MANAGER_H_

#include "../mesh/mesh.h"
#include "../animation/animation.h"
#include "../billboard/billboard.h"
#include "../util/util.h"

#ifndef SUBMESH_TYPE_NORMAL
#define SUBMESH_TYPE_NORMAL 0
#define SUBMESH_TYPE_SINGLE 1
#define SUBMESH_TYPE_BILLBD 2
#define SUBMESH_TYPE_ANIMAT 3
#define SUBMESH_TYPE_TRANSP 4
#endif

struct MeshGroup {
	int* subNormals;
	int* subSingles;
	int* subTransps;
	uint objectCount;
	MeshGroup(int nhigh, int nmid, int nlow, int nanim, int shigh, int smid, int slow, int sbill) {
		subNormals = (int*)malloc(4 * sizeof(int));
		subSingles = (int*)malloc(4 * sizeof(int));
		subTransps = (int*)malloc(4 * sizeof(int));
		objectCount = 0;
		subNormals[0] = nhigh, subNormals[1] = nmid, subNormals[2] = nlow, subNormals[3] = nanim;
		subSingles[0] = shigh, subSingles[1] = smid, subSingles[2] = slow, subSingles[3] = sbill;
		subTransps[0] = -1, subTransps[1] = -1, subTransps[2] = -1, subTransps[3] = -1;
	}
	MeshGroup(const MeshGroup* rhs) {
		subNormals = (int*)malloc(4 * sizeof(int));
		subSingles = (int*)malloc(4 * sizeof(int));
		subTransps = (int*)malloc(4 * sizeof(int));
		objectCount = 0;
		memcpy(subNormals, rhs->subNormals, 4 * sizeof(int));
		memcpy(subSingles, rhs->subSingles, 4 * sizeof(int));
		memcpy(subTransps, rhs->subTransps, 4 * sizeof(int));
	}
	~MeshGroup() {
		free(subNormals);
		free(subSingles);
		free(subTransps);
	}
	bool equals(const MeshGroup* rhs) {
		for (int i = 0; i < 4; ++i) {
			if (rhs->subNormals[i] != subNormals[i]) return false;
			if (rhs->subSingles[i] != subSingles[i]) return false;
			if (rhs->subTransps[i] != subTransps[i]) return false;
		}
		return true;
	}
};

struct SubMesh {
	std::string name;
	Mesh* mesh; 
	Animation* animat; 
	int type;
	uint objectCount;
	Indirect* indirect;
	SubMesh(Mesh* msh, int typ) {
		mesh = msh;
		animat = NULL;
		type = typ;
		std::string ext = "";
		indirect = (Indirect*)malloc(sizeof(Indirect));
		switch (type) {
			case SUBMESH_TYPE_NORMAL:
				ext = "_normal";
				indirect->count = mesh->normalFaces[0]->count;
				break;
			case SUBMESH_TYPE_SINGLE:
				ext = "_single";
				indirect->count = mesh->singleFaces[0]->count;
				break;
			case SUBMESH_TYPE_BILLBD:
				ext = "_billbd";
				indirect->count = mesh->singleFaces[0]->count;
				break;
			case SUBMESH_TYPE_TRANSP:
				ext = "_transp";
				indirect->count = mesh->transpFaces[0]->count;
				break;
		}
		name = mesh->getName() + ext;
		objectCount = 0;
	}
	SubMesh(Animation* anim) {
		mesh = NULL;
		animat = anim;
		type = SUBMESH_TYPE_ANIMAT;
		name = animat->getName() + "_animat";
		objectCount = 0;
		indirect = (Indirect*)malloc(sizeof(Indirect));
		indirect->count = animat->aIndices.size();
	}
	~SubMesh() {
		free(indirect);
	}
};

struct OffsetInfo {
	uint vertexOffset;
	uint indexOffset;
};

inline bool SubMeshValid(int subMeshid) {
	return subMeshid >= 0;
}

class Object;
class StaticObject;
class AnimationObject;
class MeshManager {
public:
	std::vector<Mesh*> meshs; // used for mesh vbo
	std::vector<Animation*> animats; // used for animation vbo
	std::map<Mesh*, OffsetInfo*> meshOffsets; // global mesh vertex & index offsets, use this with submesh single/normal FaceBuffer to get indirect offsets
	std::map<Animation*, OffsetInfo*> animatOffsets; // global animation vertex & index offsets, use this to get indirect offsets
	std::vector<SubMesh*> normalMeshs;
	std::vector<SubMesh*> singleMeshs;
	std::vector<SubMesh*> billbdMeshs;
	std::vector<SubMesh*> animatMeshs;
	std::vector<SubMesh*> transpMeshs;
	std::vector<MeshGroup*> meshGroups;
	std::vector<Billboard*> billboards;
public:
	MeshManager();
	~MeshManager();
public:
	void release();
	void addObject(Object* object);
	void showLog();
private:
	uint getMeshGroupId(const MeshGroup* group);
	int getSubMeshId(Mesh* mesh, int typ);
	int getSubMeshId(Animation* anim);
	int getBoardId(Billboard* board);
	void addObjectMesh(StaticObject* object);
	void addObjectAnim(AnimationObject* object);
	bool isMeshIn(const std::vector<Mesh*>& list, const Mesh* mesh);
	bool isAnimIn(const std::vector<Animation*>& list, const Animation* anim);
	void addSubMeshObjectCount(std::vector<SubMesh*>& list, int mid);
	void addGroupObjectCount(std::vector<MeshGroup*>& list, uint gid);
};

#endif
