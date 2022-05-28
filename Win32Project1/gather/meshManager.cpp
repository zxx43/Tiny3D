#include "meshManager.h"
#include "../object/staticObject.h"
#include "../object/animationObject.h"

#define MAX_COUNT_PER_SUB 16384
#define MAX_COUNT_PER_GROUP 16384

MeshManager::MeshManager() {
	meshs.clear();
	animats.clear();
	meshOffsets.clear();
	animatOffsets.clear();
	billboards.clear();
	meshGroups.clear();
	normalMeshs.clear();
	singleMeshs.clear();
	billbdMeshs.clear();
	animatMeshs.clear();
}

MeshManager::~MeshManager() {
	release();
}

void MeshManager::release() {
	meshs.clear();
	animats.clear();

	std::map<Mesh*, OffsetInfo*>::iterator itMesh;
	for (itMesh = meshOffsets.begin(); itMesh != meshOffsets.end(); ++itMesh)
		free(itMesh->second);
	meshOffsets.clear();
	std::map<Animation*, OffsetInfo*>::iterator itAnim;
	for (itAnim = animatOffsets.begin(); itAnim != animatOffsets.end(); ++itAnim)
		free(itAnim->second);
	animatOffsets.clear();

	for (uint i = 0; i < billboards.size(); ++i)
		delete billboards[i];
	billboards.clear();

	for (uint i = 0; i < meshGroups.size(); ++i)
		delete meshGroups[i];
	meshGroups.clear();

	for (uint i = 0; i < normalMeshs.size(); ++i)
		delete normalMeshs[i];
	normalMeshs.clear();
	for (uint i = 0; i < singleMeshs.size(); ++i)
		delete singleMeshs[i];
	singleMeshs.clear();
	for (uint i = 0; i < billbdMeshs.size(); ++i)
		delete billbdMeshs[i];
	billbdMeshs.clear();
	for (uint i = 0; i < animatMeshs.size(); ++i)
		delete animatMeshs[i];
	animatMeshs.clear();
}

uint MeshManager::getMeshGroupId(const MeshGroup* group) {
	uint target;
	bool find = false;
	for (uint i = 0; i < meshGroups.size(); ++i) {
		MeshGroup* stored = meshGroups[i];
		if (stored->equals(group)) {
			target = i;
			find = true;
			break;
		}
	}
	if (!find) {
		target = meshGroups.size();
		meshGroups.push_back(new MeshGroup(group));
	}
	return target;
}

int MeshManager::getSubMeshId(Mesh* mesh, int typ) {
	int target = -1;
	if (!mesh) return target;
	else if (mesh->isBillboard && typ != SUBMESH_TYPE_BILLBD) return target;
	else if (mesh->isBillboard && typ == SUBMESH_TYPE_BILLBD) {
		for (uint i = 0; i < billbdMeshs.size(); ++i) {
			SubMesh* sub = billbdMeshs[i];
			if (sub->mesh == mesh) {
				target = (int)i;
				break;
			}
		}
		if (target < 0) {
			target = (int)billbdMeshs.size();
			billbdMeshs.push_back(new SubMesh(mesh, typ));
		}
	} else if (!mesh->isBillboard) {
		if (typ == SUBMESH_TYPE_BILLBD) return target;
		else if (typ == SUBMESH_TYPE_NORMAL && mesh->normalFaces.size() > 0) {
			for (uint i = 0; i < normalMeshs.size(); ++i) {
				SubMesh* sub = normalMeshs[i];
				if (sub->mesh == mesh) {
					target = (int)i;
					break;
				}
			}
			if (target < 0) {
				target = (int)normalMeshs.size();
				normalMeshs.push_back(new SubMesh(mesh, typ));
			}
		} else if (typ == SUBMESH_TYPE_SINGLE && mesh->singleFaces.size() > 0) {
			for (uint i = 0; i < singleMeshs.size(); ++i) {
				SubMesh* sub = singleMeshs[i];
				if (sub->mesh == mesh) {
					target = (int)i;
					break;
				}
			}
			if (target < 0) {
				target = (int)singleMeshs.size();
				singleMeshs.push_back(new SubMesh(mesh, typ));
			}
		}
	}
	return target;
}

int MeshManager::getSubMeshId(Animation* anim) {
	int target = -1;
	if (!anim) return target;
	for (uint i = 0; i < animatMeshs.size(); ++i) {
		SubMesh* sub = animatMeshs[i];
		if (sub->animat == anim) {
			target = (int)i;
			break;
		}
	}
	if (target < 0) {
		target = (int)animatMeshs.size();
		animatMeshs.push_back(new SubMesh(anim));
	}
	return target;
}

int MeshManager::getBoardId(Billboard* board) {
	int target = -1;
	if (board) {
		for (uint i = 0; i < billboards.size(); ++i) {
			Billboard* stored = billboards[i];
			if (stored->equals(board)) {
				target = (int)i;
				break;
			}
		}
		if (target < 0) {
			target = (int)billboards.size();
			billboards.push_back(new Billboard(*board));
		}
	}
	return target;
}

bool MeshManager::isMeshIn(const std::vector<Mesh*>& list, const Mesh* mesh) {
	for (uint i = 0; i < list.size(); ++i)
		if (mesh == list[i]) return true;
	return false;
}

bool MeshManager::isAnimIn(const std::vector<Animation*>& list, const Animation* anim) {
	for (uint i = 0; i < list.size(); ++i)
		if (anim == list[i]) return true;
	return false;
}

void MeshManager::addObjectMesh(StaticObject* object) {
	Mesh* high = object->mesh;
	Mesh* mid = object->meshMid;
	Mesh* low = object->meshLow;
	if (high) {
		if (!isMeshIn(meshs, high)) meshs.push_back(high);
		if (meshOffsets.find(high) == meshOffsets.end())
			meshOffsets.insert(std::pair<Mesh*, OffsetInfo*>(high, (OffsetInfo*)malloc(sizeof(OffsetInfo))));
	}
	if (mid) {
		if (!isMeshIn(meshs, mid)) meshs.push_back(mid);
		if (meshOffsets.find(mid) == meshOffsets.end())
			meshOffsets.insert(std::pair<Mesh*, OffsetInfo*>(mid, (OffsetInfo*)malloc(sizeof(OffsetInfo))));
	}
	if (low) {
		if (!isMeshIn(meshs, low)) meshs.push_back(low);
		if (meshOffsets.find(low) == meshOffsets.end())
			meshOffsets.insert(std::pair<Mesh*, OffsetInfo*>(low, (OffsetInfo*)malloc(sizeof(OffsetInfo))));
	}
}

void MeshManager::addObjectAnim(AnimationObject* object) {
	Animation* anim = object->animation;
	if (!isAnimIn(animats, anim)) animats.push_back(anim);
	if (animatOffsets.find(anim) == animatOffsets.end())
		animatOffsets.insert(std::pair<Animation*, OffsetInfo*>(anim, (OffsetInfo*)malloc(sizeof(OffsetInfo))));

}

void MeshManager::addSubMeshObjectCount(std::vector<SubMesh*>& list, int mid) {
	if (SubMeshValid(mid) && list[mid]->objectCount < MAX_COUNT_PER_SUB) list[mid]->objectCount++;
}

void MeshManager::addGroupObjectCount(std::vector<MeshGroup*>& list, uint gid) {
	if (list[gid]->objectCount < MAX_COUNT_PER_GROUP) list[gid]->objectCount++;
}

void MeshManager::addObject(Object* object) {
	if (object->type == OBJ_TYPE_STATIC) {
		StaticObject* sobj = (StaticObject*)object;

		int subHighNormal = getSubMeshId(sobj->mesh, SUBMESH_TYPE_NORMAL);
		int subHighSingle = getSubMeshId(sobj->mesh, SUBMESH_TYPE_SINGLE);
		int subMidNormal = getSubMeshId(sobj->meshMid, SUBMESH_TYPE_NORMAL);
		int subMidSingle = getSubMeshId(sobj->meshMid, SUBMESH_TYPE_SINGLE);
		int subLowNormal = getSubMeshId(sobj->meshLow, SUBMESH_TYPE_NORMAL); 
		int subLowSingle = getSubMeshId(sobj->meshLow, SUBMESH_TYPE_SINGLE); 
		int subBill = getSubMeshId(sobj->meshLow, SUBMESH_TYPE_BILLBD);

		addSubMeshObjectCount(normalMeshs, subHighNormal);
		addSubMeshObjectCount(singleMeshs, subHighSingle);
		if (sobj->meshMid && sobj->meshMid != sobj->mesh) {
			addSubMeshObjectCount(normalMeshs, subMidNormal);
			addSubMeshObjectCount(singleMeshs, subMidSingle);
		}
		if (sobj->meshLow && sobj->meshLow != sobj->meshMid && sobj->meshLow != sobj->mesh) {
			addSubMeshObjectCount(normalMeshs, subLowNormal);
			addSubMeshObjectCount(singleMeshs, subLowSingle);
			addSubMeshObjectCount(billbdMeshs, subBill);
		}

		MeshGroup group(subHighNormal, subMidNormal, subLowNormal, -1, subHighSingle, subMidSingle, subLowSingle, subBill);
		object->groupid = getMeshGroupId(&group);
		addGroupObjectCount(meshGroups, object->groupid);

		addObjectMesh(sobj);
	} else if (object->type == OBJ_TYPE_ANIMAT) {
		AnimationObject* aobj = (AnimationObject*)object;
		
		int subAnim = getSubMeshId(aobj->animation);
		addSubMeshObjectCount(animatMeshs, subAnim);

		MeshGroup group(-1, -1, -1, subAnim, -1, -1, -1, -1);
		object->groupid = getMeshGroupId(&group);
		addGroupObjectCount(meshGroups, object->groupid);

		addObjectAnim(aobj);
	}
	object->boardid = getBoardId(object->billboard);
}

void MeshManager::showLog() {
	for (uint i = 0; i < meshGroups.size(); ++i) {
		MeshGroup* group = meshGroups[i];
		int n0 = group->subNormals[0], n1 = group->subNormals[1], n2 = group->subNormals[2], n3 = group->subNormals[3];
		int s0 = group->subSingles[0], s1 = group->subSingles[1], s2 = group->subSingles[2], s3 = group->subSingles[3];
		std::string name0 = SubMeshValid(n0) ? normalMeshs[n0]->name + " * " + std::to_string(normalMeshs[n0]->objectCount) : "invalid * -1";
		std::string name1 = SubMeshValid(n1) ? normalMeshs[n1]->name + " * " + std::to_string(normalMeshs[n1]->objectCount) : "invalid * -1";
		std::string name2 = SubMeshValid(n2) ? normalMeshs[n2]->name + " * " + std::to_string(normalMeshs[n2]->objectCount) : "invalid * -1";
		std::string name3 = SubMeshValid(n3) ? animatMeshs[n3]->name + " * " + std::to_string(animatMeshs[n3]->objectCount) : "invalid * -1";
		std::string name4 = SubMeshValid(s0) ? singleMeshs[s0]->name + " * " + std::to_string(singleMeshs[s0]->objectCount) : "invalid * -1";
		std::string name5 = SubMeshValid(s1) ? singleMeshs[s1]->name + " * " + std::to_string(singleMeshs[s1]->objectCount) : "invalid * -1";
		std::string name6 = SubMeshValid(s2) ? singleMeshs[s2]->name + " * " + std::to_string(singleMeshs[s2]->objectCount) : "invalid * -1";
		std::string name7 = SubMeshValid(s3) ? billbdMeshs[s3]->name + " * " + std::to_string(billbdMeshs[s3]->objectCount) : "invalid * -1";
		printf("Group:\n");
		printf("%s, %s, %s, %s\n", name0.data(), name1.data(), name2.data(), name3.data());
		printf("%s, %s, %s, %s\n\n", name4.data(), name5.data(), name6.data(), name7.data());
	}

	printf("Billboard: %d\n\n", billboards.size());
	for (uint i = 0; i < billboards.size(); ++i) {
		Billboard* board = billboards[i];
		printf("Board:\n");
		printf("%f %f %d\n\n", board->data[0], board->data[1], board->material);
	}
}