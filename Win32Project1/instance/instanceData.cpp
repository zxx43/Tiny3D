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

void InstanceData::addInstance(Object* object, bool uniformScale) {
	if (transformsFull && instance) {
		bool valid = instance->insId != InvalidInsId || instance->insSingleId != InvalidInsId || instance->insBillId != InvalidInsId;
		if (valid) {
			memcpy(transformsFull + (count * 16), object->transformsFull, 12 * sizeof(buff));
			transformsFull[count * 16 + 12] = instance->insId;
			transformsFull[count * 16 + 13] = instance->insSingleId;
			transformsFull[count * 16 + 14] = instance->insBillId;
			transformsFull[count * 16 + 15] = object->material;

			if (!uniformScale) {
				vec3 scale(object->scaleMat[0], object->scaleMat[5], object->scaleMat[10]);
				float pScale = PackVec2Float(scale);
				transformsFull[count * 16 + 3] = pScale;
			}

			if (instance->isBillboard) {
				memcpy(transformsFull + (count * 16) + 4, object->billboard->data, 3 * sizeof(buff));
				transformsFull[count * 16 + 15] = object->billboard->material;
			} else {
				vec4 quat(transformsFull[count * 16 + 4], transformsFull[count * 16 + 5], transformsFull[count * 16 + 6], transformsFull[count * 16 + 7]);
				vec3 quat3 = EncodeQuat(quat, true);
				transformsFull[count * 16 + 4] = quat3.x;
				transformsFull[count * 16 + 5] = quat3.y;
				transformsFull[count * 16 + 6] = quat3.z;
			}
			transformsFull[count * 16 + 7] = uniformScale ? 1.0 : -1.0;

			count++;
		}
	} 
}