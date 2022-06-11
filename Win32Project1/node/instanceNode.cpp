#include "instanceNode.h"
#include "../util/util.h"
#include "../scene/scene.h"
using namespace std;

InstanceNode::InstanceNode(const vec3& position):Node(position, vec3(0, 0, 0)) {
	type = TYPE_INSTANCE;

	needCreateDrawcall = false;
	needUpdateDrawcall = false;
}

InstanceNode::~InstanceNode() {

}

void InstanceNode::addObject(Scene* scene, Object* object) {
	Node::addObject(scene, object);
	if (object->belong) object->belong->addObject(object);
}

Object* InstanceNode::removeObject(Object* object) {
	if (object->belong) object->belong->removeObject(object);
	return Node::removeObject(object);
}

void InstanceNode::addObjects(Scene* scene,Object** objectArray,int count) {
	for(int i=0;i<count;i++)
		this->addObject(scene,objectArray[i]);
}

void InstanceNode::prepareDrawcall() {
	needCreateDrawcall = false;
}
