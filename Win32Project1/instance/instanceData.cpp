#include "instance.h"

InstanceData::InstanceData(Mesh* mesh, Object* obj, int maxCount, InstanceState* insState) {
	insMesh = mesh;
	count = 0;
	matrices = NULL;
	billboards = NULL;
	positions = NULL;
	object = obj;
	instance = NULL;
	state = new InstanceState(*insState);

	if (mesh->isBillboard) {
		billboards = (float*)malloc(maxCount * 4 * sizeof(float));
		positions = (float*)malloc(maxCount * 3 * sizeof(float));
		memset(billboards, 0, maxCount * 4 * sizeof(float));
		memset(positions, 0, maxCount * 3 * sizeof(float));
	} else if (state->simple) {
		matrices = (float*)malloc(maxCount * 4 * sizeof(float));
		memset(matrices, 0, maxCount * 4 * sizeof(float));
	} else {
		matrices = (float*)malloc(maxCount * 12 * sizeof(float));
		memset(matrices, 0, maxCount * 12 * sizeof(float));
	}

	subGroup.clear();
	groupToMerge = 0;
}

InstanceData::~InstanceData() {
	if (matrices) free(matrices);
	if (billboards) free(billboards);
	if (positions) free(positions);
	if (instance) delete instance;
	delete state;
	subGroup.clear();
}

void InstanceData::resetInstance() {
	count = 0;
	groupToMerge = 0;
}

void InstanceData::addInstance(Object* object) {
	if (matrices && !state->simple)
		memcpy(matrices + (count * 12), object->transformTransposed.entries, 12 * sizeof(float));
	else if (matrices && state->simple)
		memcpy(matrices + (count * 4), object->transforms, 4 * sizeof(float));
	else {
		Material* mat = NULL;
		if (MaterialManager::materials)
			mat = MaterialManager::materials->find(object->billboard->material);

		billboards[count * 4 + 0] = object->billboard->data[0];
		billboards[count * 4 + 1] = object->billboard->data[1];
		if (mat) {
			billboards[count * 4 + 2] = mat->texOfs1.x;
			billboards[count * 4 + 3] = mat->texOfs1.y;
		}

		memcpy(positions + (count * 3), object->transformMatrix.entries + 12, 3 * sizeof(float));
	}

	count++;
}

void InstanceData::doMergeData(InstanceData* data) {
	if (data->matrices && data->state->simple)
		memcpy(matrices + (count * 4), data->matrices, data->count * 4 * sizeof(float));
	else if (data->matrices)
		memcpy(matrices + (count * 12), data->matrices, data->count * 12 * sizeof(float));
	else if (data->billboards) {
		memcpy(billboards + (count * 4), data->billboards, data->count * 4 * sizeof(float));
		memcpy(positions + (count * 3), data->positions, data->count * 3 * sizeof(float));
	}
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