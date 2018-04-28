#include "instanceNode.h"
#include "../render/instanceDrawcall.h"
#include "../util/util.h"
using namespace std;

InstanceNode::InstanceNode(const VECTOR3D& position):Node(position, VECTOR3D(0, 0, 0)) {
	type = TYPE_INSTANCE;
}

InstanceNode::~InstanceNode() {
		
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

void InstanceNode::prepareDrawcall() {
	needCreateDrawcall = false;
}

void InstanceNode::updateRenderData(Camera* camera, int pass) {

}

void InstanceNode::updateDrawcall(int pass) {
	needUpdateDrawcall = false;
}
