#include "instanceNode.h"
#include "../render/instanceDrawcall.h"
#include "../util/util.h"

InstanceNode::InstanceNode(const VECTOR3D& position):Node(position, VECTOR3D(0, 0, 0)) {
	instance = NULL;
	type = TYPE_INSTANCE;
}

InstanceNode::~InstanceNode() {
	if(instance)
		delete instance;
	instance=NULL;
}

void InstanceNode::addObjects(Object** objectArray,int count) {
	for(int i=0;i<count;i++)
		addObject(objectArray[i]);
}

void InstanceNode::createInstance() {
	if (instance) delete instance;
	Mesh* instanceMesh=objects[0]->mesh;
	instance=new Instance(instanceMesh);
	instance->initInstanceBuffers(objects[0]->material,instanceMesh->vertexCount,instanceMesh->indexCount);
	MATRIX4X4 nodeTransform; nodeTransform.LoadIdentity();
	recursiveTransform(nodeTransform);
	for(unsigned int i=0;i<objects.size();i++) {
		Object* object=objects[i];
		MATRIX4X4 transformMatrix=nodeTransform*object->localTransformMatrix;
		instance->pushObjectToInstances(transformMatrix,object->normalMatrix);
	}
	instance->initMatrices();

	if (drawcall) delete drawcall;
	drawcall=new InstanceDrawcall(instance);
	drawcall->setSide(singleSide);
}

void InstanceNode::prepareDrawcall() {
	createInstance();
	needCreateDrawcall = false;
}

void InstanceNode::updateDrawcall(bool updateNormal) {
	if (!instance) createInstance();
	else {
		MATRIX4X4 nodeTransform; nodeTransform.LoadIdentity();
		recursiveTransform(nodeTransform);
		for (unsigned int i = 0; i < objects.size(); i++) {
			Object* object = objects[i];
			MATRIX4X4 transformMatrix = nodeTransform*object->localTransformMatrix;
			if (updateNormal) {
				MATRIX4X4 normalMatrix = object->normalMatrix;
				instance->updateMatricesBuffer(i, transformMatrix, &normalMatrix);
			} else
				instance->updateMatricesBuffer(i, transformMatrix, NULL);
		}

		if (!drawcall) {
			drawcall = new InstanceDrawcall(instance);
			drawcall->setSide(singleSide);
		} else
			((InstanceDrawcall*)drawcall)->updateMatrices(instance, updateNormal);
	}
	needUpdateDrawcall = false;
}
