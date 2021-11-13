#include "instance.h"

InstanceData::InstanceData(Mesh* mesh, Object* obj, int maxCount) {
	insMesh = mesh;
	count = 0, maxInsCount = maxCount;
	transformsFull = NULL;
	object = obj;
	instance = NULL;

	transformsFull = (buff*)malloc(maxCount * 16 * sizeof(buff));
	memset(transformsFull, 0, maxCount * 16 * sizeof(buff));
}

InstanceData::~InstanceData() {
	if (transformsFull) free(transformsFull);
	if (instance) delete instance;
}

void InstanceData::resetInstance() {
	count = 0;
}

void InstanceData::addInstance(Object* object) {
	if (transformsFull && instance) {
		bool valid = instance->insId != InvalidInsId || instance->insSingleId != InvalidInsId || instance->insBillId != InvalidInsId;
		if (valid) {
			memcpy(transformsFull + (count * 16), object->transformsFull, 12 * sizeof(buff));
			transformsFull[count * 16 + 12] = instance->insId;
			transformsFull[count * 16 + 13] = instance->insSingleId;
			transformsFull[count * 16 + 14] = instance->insBillId;
			transformsFull[count * 16 + 15] = object->material;
			if (instance->isBillboard) {
				if (object->billboard->data[2] < 0) {
					Material* mat = NULL;
					if (MaterialManager::materials)
						mat = MaterialManager::materials->find(object->billboard->material);
					object->billboard->data[2] = mat ? mat->texids.x : 0.0;
				}
				memcpy(transformsFull + (count * 16) + 4, object->billboard->data, 3 * sizeof(buff));
			}
			count++;
		}
	} 
}