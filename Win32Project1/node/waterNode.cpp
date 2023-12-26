#include "waterNode.h"
#include "../scene/scene.h"

WaterNode::WaterNode(const vec3& position) : StaticNode(position) {
	centerX = 0, centerZ = 0;
	type = TYPE_WATER;
}

WaterNode::~WaterNode() {

}

void WaterNode::putCenter() {
	centerX = position.x;
	centerZ = position.z;
}

void WaterNode::addObject(Scene* scene, Object* object) {
	Node::addObject(scene, object);
	putCenter();
}

Object* WaterNode::getObject() {
	if (objects.size() > 0) return objects[0];
	else return NULL;
}

void WaterNode::moveWaterWithCamera(Scene* scene, const Camera* camera) {
	float dx = camera->position.x;
	float dz = camera->position.z;
	translateNode(scene, centerX + dx, position.y, centerZ + dz);
}