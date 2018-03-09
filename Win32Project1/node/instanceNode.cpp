#include "instanceNode.h"
#include "../render/instanceDrawcall.h"
#include "../util/util.h"
using namespace std;

InstanceNode::InstanceNode(const VECTOR3D& position):Node(position, VECTOR3D(0, 0, 0)) {
	type = TYPE_INSTANCE;
}

InstanceNode::~InstanceNode() {
}

void InstanceNode::addObjects(Object** objectArray,int count) {
	for(int i=0;i<count;i++)
		addObject(objectArray[i]);
}

void InstanceNode::prepareDrawcall() {
	needCreateDrawcall = false;
}

void InstanceNode::updateRenderData(Camera* camera, int pass) {

}

void InstanceNode::updateDrawcall(int pass) {
	needUpdateDrawcall = false;
}
