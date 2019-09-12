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

void WaterNode::moveWaterWithCamera(Camera* camera) {
	float dx = camera->position.x;
	float dz = camera->position.z;
	translateNode(centerX + dx, position.y, centerZ + dz);
}