#include "instanceNode.h"
#include "../render/instanceDrawcall.h"
#include "../util/util.h"
#include "../scene/scene.h"
using namespace std;

InstanceNode::InstanceNode(const vec3& position):Node(position, vec3(0, 0, 0)) {
	type = TYPE_INSTANCE;
	dynamic = true;
	instance = NULL;
	isGroup = false;
	groupBuffer = NULL;
}

InstanceNode::~InstanceNode() {
	if (instance) delete instance; instance = NULL;
	if (groupBuffer) delete groupBuffer; groupBuffer = NULL;
}

void InstanceNode::addObject(Scene* scene, Object* object) {
	Node::addObject(scene, object);
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

	scene->addObject(object);
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

void InstanceNode::addObjects(Scene* scene,Object** objectArray,int count) {
	for(int i=0;i<count;i++)
		this->addObject(scene,objectArray[i]);
}

void InstanceNode::prepareGroup() {
	if (groupBuffer) return;
	groupBuffer = new InstanceData(objects[0]->mesh, objects[0], objects.size());
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
			instance = new Instance(mesh, dynamic);
			instance->initInstanceBuffers(objects[0], mesh->vertexCount, mesh->indexCount, objects.size(), true);
			for (uint i = 0; i < objects.size(); i++)
				instance->addObject(objects[i], i);

			drawcall = new InstanceDrawcall(instance);
			InstanceDrawcall* insDC = (InstanceDrawcall*)drawcall;
			insDC->objectToPrepare = objects.size();
		}
	}

	needCreateDrawcall = false;
}
