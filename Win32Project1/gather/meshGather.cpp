#include "meshGather.h"

MeshGather::MeshGather(MeshManager* manager) {
	meshManager = manager;

	meshData = new MeshData();
	for (uint i = 0; i < manager->meshs.size(); ++i) {
		Mesh* mesh = manager->meshs[i];
		manager->meshOffsets[mesh]->vertexOffset = (uint)meshData->vertexCount;
		manager->meshOffsets[mesh]->indexOffset = (uint)meshData->indexCount;

		MeshData data(mesh);
		meshData->append(&data);
	}

	animData = new MeshData();
	for (uint i = 0; i < manager->animats.size(); ++i) {
		Animation* anim = manager->animats[i];
		manager->animatOffsets[anim]->vertexOffset = (uint)animData->vertexCount;
		manager->animatOffsets[anim]->indexOffset = (uint)animData->indexCount;
		
		MeshData data(anim);
		animData->append(&data);
	}

	normals = new IndirectData();
	singles = new IndirectData();
	billbds = new IndirectData();
	animats = new IndirectData();
	//todo
	for (uint i = 0; i < manager->normalMeshs.size(); ++i)
		addIndirect(manager->normalMeshs[i], normals);
	for (uint i = 0; i < manager->singleMeshs.size(); ++i)
		addIndirect(manager->singleMeshs[i], singles);
	for (uint i = 0; i < manager->billbdMeshs.size(); ++i)
		addIndirect(manager->billbdMeshs[i], billbds);
	for (uint i = 0; i < manager->animatMeshs.size(); ++i)
		addIndirect(manager->animatMeshs[i], animats);
	//todo

	groupData = new MeshGroupData();
	for (uint i = 0; i < manager->meshGroups.size(); ++i)
		groupData->append(manager->meshGroups[i]);

	boardData = new BillboardData();
	for (uint i = 0; i < manager->billboards.size(); ++i)
		boardData->append(manager->billboards[i]);

	maxSubCount = normals->count > singles->count ? normals->count : singles->count;
	maxSubCount = maxSubCount > billbds->count ? maxSubCount : billbds->count;
	maxSubCount = maxSubCount > animats->count ? maxSubCount : animats->count;
}

MeshGather::~MeshGather() {
	delete normals;
	delete singles;
	delete billbds;
	delete animats;
	//todo
	delete groupData;
	delete boardData;
	delete meshData;
	delete animData;
}

void MeshGather::addIndirect(SubMesh* sub, IndirectData* indirectData) {
	if (sub->mesh) {
		Mesh* mesh = sub->mesh;
		sub->indirect->baseVertex = meshManager->meshOffsets[mesh]->vertexOffset;
		if (sub->type == SUBMESH_TYPE_NORMAL)
			sub->indirect->firstIndex = meshManager->meshOffsets[mesh]->indexOffset + mesh->normalFaces[0]->start;
		else if (sub->type == SUBMESH_TYPE_SINGLE || sub->type == SUBMESH_TYPE_BILLBD)
			sub->indirect->firstIndex = meshManager->meshOffsets[mesh]->indexOffset + mesh->singleFaces[0]->start;
		//todo
	} else {
		Animation* anim = sub->animat;
		sub->indirect->baseVertex = meshManager->animatOffsets[anim]->vertexOffset;
		sub->indirect->firstIndex = meshManager->animatOffsets[anim]->indexOffset;
	}
	indirectData->append(sub->indirect);
}

void MeshGather::showLog() {
	printf("Indirect info:\n");
	printf("sub normal count: %d\n", normals->count);
	printf("sub single count: %d\n", singles->count);
	printf("sub billbd count: %d\n", billbds->count);
	printf("sub animat count: %d\n", animats->count);
	//todo
	printf("max sub count: %d\n\n", maxSubCount);
}