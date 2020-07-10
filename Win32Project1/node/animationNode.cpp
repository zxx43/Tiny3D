#include "animationNode.h"
#include "../util/util.h"
#include "../scene/scene.h"
#include <string.h>

AnimationNode::AnimationNode(const vec3& boundingSize):
		Node(vec3(0, 0, 0), boundingSize) {
	animation = NULL;
	type = TYPE_ANIMATE;

	needCreateDrawcall = false;
	needUpdateDrawcall = false;
	needUpdateNode = false;
}

AnimationNode::~AnimationNode() {}

void AnimationNode::setAnimation(Scene* scene, Animation* anim) {
	animation = anim;
	AnimationObject* object = new AnimationObject(animation);
	Node::addObject(scene, object);
	scene->addObject(object);
	scene->addPlay(this);
}

void AnimationNode::prepareDrawcall() {
	needCreateDrawcall = false;
}

void AnimationNode::updateDrawcall() {
	needUpdateDrawcall = false;
}

void AnimationNode::updateRenderData() {
}

void AnimationNode::animate(float velocity) {
	AnimationObject* object = getObject();
	if (object) object->animate(velocity);
}

AnimationObject* AnimationNode::getObject() {
	if (objects.size() > 0)
		return (AnimationObject*)objects[0];
	return NULL;
}

void AnimationNode::translateNodeCenterAtWorld(Scene* scene, float x, float y, float z) {
	vec3 beforeWorldCenter = boundingBox->position;
	vec3 offset = vec3(x, y, z) - beforeWorldCenter;
	vec3 dPosition = position + offset;
	translateNode(scene, dPosition.x, dPosition.y, dPosition.z);
}

void AnimationNode::translateNode(Scene* scene, float x, float y, float z) {
	position = vec3(x, y, z);
	recursiveTransform(nodeTransform);
	
	if (!needUpdateNode) {
		setUpdate(true);
		scene->animNodeToUpdate.push_back(this);
	}
	// todo update collision shape
}

void AnimationNode::rotateNodeObject(Scene* scene, float ax, float ay, float az) {
	AnimationObject* object = (AnimationObject*)objects[0];
	object->setRotation(ax, ay, az);

	if (!needUpdateNode) {
		setUpdate(true);
		scene->animNodeToUpdate.push_back(this);
	}
	// todo update collision shape
}
