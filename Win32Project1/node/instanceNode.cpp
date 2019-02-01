#include "instanceNode.h"
#include "../render/instanceDrawcall.h"
#include "../util/util.h"
using namespace std;

InstanceNode::InstanceNode(const VECTOR3D& position):Node(position, VECTOR3D(0, 0, 0)) {
	type = TYPE_INSTANCE;
	dynamic = true;
	instance = NULL;
	simple = false;
	isGroup = false;
	groupBuffer = NULL;
}

InstanceNode::~InstanceNode() {
	if (instance) delete instance; instance = NULL;
	if (groupBuffer) delete groupBuffer; groupBuffer = NULL;
}

void InstanceNode::addObject(Object* object) {
	Node::addObject(object);
	if (Instance::instanceTable.find(object->mesh) == Instance::instanceTable.end())
		Instance::instanceTable[object->mesh] = 1;
	else
		Instance::instanceTable[object->mesh]++;

	if (object->meshMid) {
		if (Instance::instanceTable.find(object->meshMid) == Instance::instanceTable.end())
			Instance::instanceTable[object->meshMid] = 1;
		else
			Instance::instanceTable[object->meshMid]++;
	}

	if (object->meshLow) {
		if (Instance::instanceTable.find(object->meshLow) == Instance::instanceTable.end())
			Instance::instanceTable[object->meshLow] = 1;
		else
			Instance::instanceTable[object->meshLow]++;
	}
}

Object* InstanceNode::removeObject(Object* object) {
	Object* object2Remove = Node::removeObject(object);
	if (object2Remove) {
		Instance::instanceTable[object2Remove->mesh]--;
		if (object2Remove->meshMid)
			Instance::instanceTable[object2Remove->meshMid]--;
		if (object2Remove->meshLow)
			Instance::instanceTable[object2Remove->meshLow]--;
	}
	return object2Remove;
}

void InstanceNode::addObjects(Object** objectArray,int count) {
	for(int i=0;i<count;i++)
		this->addObject(objectArray[i]);
}

void InstanceNode::prepareGroup() {
	if (groupBuffer) return;
	groupBuffer = new InstanceData(objects[0]->mesh, objects[0], objects.size(), singleSide, simple);
	for (uint i = 0; i < objects.size(); ++i)
		groupBuffer->addInstance(objects[i]);
}

void InstanceNode::releaseGroup() {
	if (groupBuffer) {
		delete groupBuffer;
		groupBuffer = NULL;
	}
}

void InstanceNode::prepareDrawcall() {
	if (!dynamic) {
		if (!drawcall && objects.size() > 0) {
			Mesh* mesh = objects[0]->mesh;
			instance = new Instance(mesh, dynamic, simple);
			instance->singleSide = singleSide;
			instance->initInstanceBuffers(objects[0], mesh->vertexCount, mesh->indexCount, objects.size(), true);
			for (uint i = 0; i < objects.size(); i++)
				instance->addObject(objects[i], i);

			drawcall = new InstanceDrawcall(instance);
			InstanceDrawcall* insDC = (InstanceDrawcall*)drawcall;
			insDC->setSide(singleSide);
			insDC->objectToPrepare = objects.size();
		}
	}

	needCreateDrawcall = false;
}

void InstanceNode::updateRenderData() {
}

void InstanceNode::updateDrawcall() {
	needUpdateDrawcall = false;
}
