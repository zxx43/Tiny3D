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
}

AnimationNode::~AnimationNode() {
	while (objects.size() > 0) {
		Object* object = objects[objects.size() - 1];
		delete removeObject(object);
	}
}

void AnimationNode::setAnimation(Scene* scene, Animation* anim) {
	animation = anim;
	AnimationObject* object = new AnimationObject(animation);
	Node::addObject(scene, object);
	if (object->belong) {
		object->belong->addObject(object);
		object->belong->addPlay(this);
	}
}

Object* AnimationNode::removeObject(Object* object) {
	if (object->belong) {
		object->belong->removePlay(this);
		object->belong->removeObject(object);
	}
	return Node::removeObject(object);
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

void AnimationNode::translateNodeCenterAtWorld(Scene* scene, const vec3& nowWorldCenter) {
	vec3 beforeWorldCenter = GetTranslate(nodeTransform);
	vec3 offset = nowWorldCenter - beforeWorldCenter;
	vec3 dPosition = position + offset;

	position = vec3(dPosition.x, dPosition.y, dPosition.z);
	doUpdateNodeTransform(scene, true, false, true);
}

void AnimationNode::translateNodeAtWorld(Scene* scene, float x, float y, float z) {
	mat4 gParentTransform = parent->nodeTransform; // Parent node's global transform
	vec3 gParentPosition = GetTranslate(gParentTransform);
	vec3 gPosition = vec3(x, y, z);
	position = gPosition - gParentPosition;
	nodeTransform = gParentTransform * translate(position.x, position.y, position.z);
}

void AnimationNode::rotateNodeAtWorld(Scene* scene, const vec4& quat) {
	getObject()->setRotation(quat);
}

void AnimationNode::translateNode(Scene* scene, float x, float y, float z) {
	positionBefore = GetTranslate(nodeTransform);
	position = vec3(x, y, z);
	if (scene->isInited())
		doUpdateNodeTransform(scene, true, false, false);
	else 
		updateNodeTransform();
}

void AnimationNode::rotateNodeObject(Scene* scene, float ax, float ay, float az) {
	getObject()->setRotation(ax, ay, az);
	
	if (scene->isInited())
		doUpdateNodeTransform(scene, false, true, true);
}

void AnimationNode::doUpdateNodeTransform(Scene* scene, bool translate, bool rotate, bool forceTrans) {
	if (translate) {
		updateNodeTransform();

		vec3 gPosition = GetTranslate(nodeTransform); // Collision object center is node center
		if (!forceTrans)
			getObject()->collisionObject->setTranslate(gPosition, positionBefore);
		else
			getObject()->collisionObject->initTranslate(gPosition);
	} 
	if (rotate) {
		vec3 gRotation = vec3(getObject()->anglex, getObject()->angley, getObject()->anglez);
		getObject()->collisionObject->setRotateAngle(gRotation, animation->inverseYZ); // fbx inverse obb yz readback to object 
	}
}

void AnimationNode::scaleNodeObject(Scene* scene, float sx, float sy, float sz) {
	AnimationObject* object = getObject();
	object->setSize(sx, sy, sz);
}
