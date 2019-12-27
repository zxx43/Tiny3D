#include "instance.h"

InstanceData::InstanceData(Mesh* mesh, Object* obj, int maxCount) {
	insMesh = mesh;
	count = 0, maxInsCount = maxCount;
	matrices = NULL;
	transformsFull = NULL;
	billboards = NULL;
	object = obj;
	instance = NULL;

	if (mesh->isBillboard) {
		billboards = (bill*)malloc(maxCount * 6 * sizeof(bill));
		memset(billboards, 0, maxCount * 6 * sizeof(bill));
	} else {
		transformsFull = (buff*)malloc(maxCount * 12 * sizeof(buff));
		memset(transformsFull, 0, maxCount * 12 * sizeof(buff));
	}

	subGroup.clear();
	groupToMerge = 0;
}

InstanceData::~InstanceData() {
	if (matrices) free(matrices);
	if (transformsFull) free(transformsFull);
	if (billboards) free(billboards);
	if (instance) delete instance;
	subGroup.clear();
}

void InstanceData::resetInstance() {
	count = 0;
	groupToMerge = 0;
}

void InstanceData::addInstance(Object* object) {
	if (transformsFull)
		memcpy(transformsFull + (count * 12), object->transformsFull, 12 * sizeof(buff));
	else if (matrices) {
		memcpy(matrices + (count * 12) + 0, object->transforms, 4 * sizeof(float));
		memcpy(matrices + (count * 12) + 4, object->rotateQuat, 4 * sizeof(float));
		memcpy(matrices + (count * 12) + 8, object->boundInfo, 4 * sizeof(float));
	}
	else {
		Material* mat = NULL;
		if (MaterialManager::materials)
			mat = MaterialManager::materials->find(object->billboard->material);

		billboards[count * 6 + 0] = Float2Half(object->billboard->data[0]);
		billboards[count * 6 + 1] = Float2Half(object->billboard->data[1]);
		billboards[count * 6 + 2] = Float2Half(mat ? mat->texids.x : 0.0);
		billboards[count * 6 + 3] = Float2Half(object->transformMatrix.entries[12]);
		billboards[count * 6 + 4] = Float2Half(object->transformMatrix.entries[13]);
		billboards[count * 6 + 5] = Float2Half(object->transformMatrix.entries[14]);
	}

	count++;
}

void InstanceData::doMergeData(InstanceData* data) {
	if (data->transformsFull)
		memcpy(transformsFull + (count * 12), data->transformsFull, data->count * 12 * sizeof(buff));
	else if (data->matrices)
		memcpy(matrices + (count * 12), data->matrices, data->count * 12 * sizeof(float));
	else if (data->billboards) 
		memcpy(billboards + (count * 6), data->billboards, data->count * 6 * sizeof(bill));
}

void InstanceData::merge(InstanceData* data) {
	if (subGroup.size() <= groupToMerge) {
		doMergeData(data);
		subGroup.push_back(data);
	} else if (subGroup[groupToMerge] != data) {
		doMergeData(data);
		subGroup[groupToMerge] = data;
	}
	groupToMerge++;
	count += data->count;
}