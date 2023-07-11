#include "objectGather.h"
#include "../object/staticObject.h"
#include "../object/animationObject.h"
#include <assert.h>

ObjectGather::ObjectGather(const MeshManager* meshManager) {
	maxObjectSize = 0;
	for (uint i = 0; i < meshManager->meshGroups.size(); ++i) {
		MeshGroup* group = meshManager->meshGroups[i];
		maxObjectSize += group->objectCount;
	}
	inObjectBuffer = (buff*)malloc(maxObjectSize * 16 * sizeof(buff));

	maxNormalSize = 0, maxSingleSize = 0, maxBillbdSize = 0, maxAnimatSize = 0, maxTranspSize = 0;
	for (uint i = 0; i < meshManager->normalMeshs.size(); ++i)
		maxNormalSize += meshManager->normalMeshs[i]->objectCount;
	for (uint i = 0; i < meshManager->singleMeshs.size(); ++i)
		maxSingleSize += meshManager->singleMeshs[i]->objectCount;
	for (uint i = 0; i < meshManager->billbdMeshs.size(); ++i)
		maxBillbdSize += meshManager->billbdMeshs[i]->objectCount;
	for (uint i = 0; i < meshManager->animatMeshs.size(); ++i)
		maxAnimatSize += meshManager->animatMeshs[i]->objectCount;
	for (uint i = 0; i < meshManager->transpMeshs.size(); ++i)
		maxTranspSize += meshManager->transpMeshs[i]->objectCount;

	resetGroupObject();
}

ObjectGather::~ObjectGather() {
	free(inObjectBuffer);
}

void ObjectGather::addGroupObject(Object* object) {
	if (inObjectCount < maxObjectSize) {
		vec3 trans(object->transformsFull[0], object->transformsFull[1], object->transformsFull[2]);
		vec4 quat(object->transformsFull[4], object->transformsFull[5], object->transformsFull[6], object->transformsFull[7]);
		vec3 quat3 = EncodeQuat(quat, true);
		vec3 scale(object->scaleMat[0], object->scaleMat[5], object->scaleMat[10]);
		vec4 bound(object->transformsFull[8], object->transformsFull[9], object->transformsFull[10], object->transformsFull[11]);

		inObjectBuffer[inObjectCount * 16 + 0] = trans.x;
		inObjectBuffer[inObjectCount * 16 + 1] = trans.y;
		inObjectBuffer[inObjectCount * 16 + 2] = trans.z;
		inObjectBuffer[inObjectCount * 16 + 4] = quat3.x;
		inObjectBuffer[inObjectCount * 16 + 5] = quat3.y;
		inObjectBuffer[inObjectCount * 16 + 6] = quat3.z;
		inObjectBuffer[inObjectCount * 16 + 8] = bound.x;
		inObjectBuffer[inObjectCount * 16 + 9] = bound.y;
		inObjectBuffer[inObjectCount * 16 + 10] = bound.z;
		inObjectBuffer[inObjectCount * 16 + 11] = bound.w;
		inObjectBuffer[inObjectCount * 16 + 14] = object->groupid;
		inObjectBuffer[inObjectCount * 16 + 15] = object->material;

		if (object->type == OBJ_TYPE_STATIC) {
			StaticObject* sobj = (StaticObject*)object;
			inObjectBuffer[inObjectCount * 16 + 3] = scale.x;
			inObjectBuffer[inObjectCount * 16 + 7] = sobj->boardid;
			inObjectBuffer[inObjectCount * 16 + 12] = scale.y;
			inObjectBuffer[inObjectCount * 16 + 13] = scale.z;
		} else if (object->type == OBJ_TYPE_ANIMAT) {
			AnimationObject* aobj = (AnimationObject*)object;
			bool uniformScale = fabsf(scale.x - scale.y) < 0.01 && fabsf(scale.x - scale.z) < 0.01;
			if (uniformScale) {
				inObjectBuffer[inObjectCount * 16 + 3] = scale.x;
				inObjectBuffer[inObjectCount * 16 + 7] = 1.0;
			}
			else {
				inObjectBuffer[inObjectCount * 16 + 3] = PackVec2Float(scale);
				inObjectBuffer[inObjectCount * 16 + 7] = -1.0;
			}
			inObjectBuffer[inObjectCount * 16 + 12] = aobj->fid + 0.1;
			inObjectBuffer[inObjectCount * 16 + 13] = aobj->getCurFrame();
		}
		inObjectCount++;
	}

	if (inObjectCount > maxObjectSize) printf("error size %d max is %d\n", inObjectCount, maxObjectSize);
}

void ObjectGather::showLog() {
	printf("Object status:\n");
	printf("max group object %d\n", maxObjectSize);
	printf("max normal object %d\n", maxNormalSize);
	printf("max single object %d\n", maxSingleSize);
	printf("max transp object %d\n", maxTranspSize);
	printf("max billbd object %d\n", maxBillbdSize);
	printf("max animat object %d\n\n", maxAnimatSize);
}