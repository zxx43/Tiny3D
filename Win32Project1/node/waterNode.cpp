#include "waterNode.h"

WaterNode::WaterNode(const VECTOR3D& position) : StaticNode(position) {
	centerX = 0, centerZ = 0;
	type = TYPE_WATER;
}

WaterNode::~WaterNode() {

}

void WaterNode::putCenter() {
	centerX = position.x;
	centerZ = position.z;
}

void WaterNode::addObject(Object* object) {
	Node::addObject(object);
	putCenter();
}

void WaterNode::moveWaterWithCamera(Camera* camera) {
	float dx = camera->position.x;
	float dz = camera->position.z;
	translateNode(centerX + dx, position.y, centerZ + dz);
}