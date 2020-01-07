#include "instance.h"

InstanceData::InstanceData(Mesh* mesh, Object* obj, int maxCount) {
	insMesh = mesh;
	count = 0, maxInsCount = maxCount;
	transformsFull = NULL;
	billboards = NULL;
	object = obj;
	instance = NULL;

	if (mesh->isBillboard) {
		billboards = (bill*)malloc(maxCount * 6 * sizeof(bill));
		memset(billboards, 0, maxCount * 6 * sizeof(bill));
	} else {
		transformsFull = (buff*)malloc(maxCount * 16 * sizeof(buff));
		memset(transformsFull, 0, maxCount * 16 * sizeof(buff));
	}
}

InstanceData::~InstanceData() {
	if (transformsFull) free(transformsFull);
	if (billboards) free(billboards);
	if (instance) delete instance;
}

void InstanceData::resetInstance() {
	count = 0;
}

void InstanceData::addInstance(Object* object) {
	if (transformsFull) {
		memcpy(transformsFull + (count * 16), object->transformsFull, 16 * sizeof(buff));
		if (instance) {
			transformsFull[count * 16 + 12] = instance->insId;
			transformsFull[count * 16 + 13] = instance->insSingleId;
		}
	} else {
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