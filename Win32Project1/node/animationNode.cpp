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
	needUpdateAnimNode = false;
}

AnimationNode::~AnimationNode() {}

void AnimationNode::setAnimation(Scene* scene, Animation* anim) {
	animation = anim;
	AnimationObject* object = new AnimationObject(animation);
	Node::addObject(scene, object);
	scene->addObject(object);
	scene->addPlay(this);
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

void AnimationNode::pushToUpdate(Scene* scene) {
	if (!needUpdateAnimNode) {
		setUpdate(true);
		scene->animNodeToUpdate.push_back(this);
	}
}

void AnimationNode::translateNodeCenterAtWorld(Scene* scene, float x, float y, float z) {
	vec3 beforeWorldCenter = boundingBox->position;
	vec3 offset = vec3(x, y, z) - beforeWorldCenter;
	vec3 dPosition = position + offset;

	position = vec3(dPosition.x, dPosition.y, dPosition.z);
	doUpdateNodeTransform(scene, true, false, true);
}

void AnimationNode::translateNodeAtWorld(Scene* scene, float x, float y, float z) {
	//mat4 gParentTransform;
	//parent->recursiveTransform(gParentTransform); // Parent node's global transform
	mat4 gParentTransform = parent->nodeTransform; // Parent node's global transform
	vec3 gParentPosition = GetTranslate(gParentTransform);
	vec3 gPosition = vec3(x, y, z);
	vec3 lPosition = gPosition - gParentPosition;
	
	position = lPosition;
	nodeTransform = gParentTransform * translate(position.x, position.y, position.z);

	pushToUpdate(scene);
}

void AnimationNode::translateNode(Scene* scene, float x, float y, float z) {
	positionBefore = GetTranslate(nodeTransform);
	position = vec3(x, y, z);
	if (scene->isInited())
		doUpdateNodeTransform(scene, true, false, false);
}

void AnimationNode::rotateNodeObject(Scene* scene, float ax, float ay, float az) {
	rotationBefore = vec3(getObject()->anglex, getObject()->angley, getObject()->anglez);
	getObject()->setRotation(ax, ay, az);
	if (scene->isInited())
		doUpdateNodeTransform(scene, false, true, false);
}

void AnimationNode::doUpdateNodeTransform(Scene* scene, bool translate, bool rotate, bool forceTrans) {
	if (translate) {
		updateNodeTransform();
		pushToUpdate(scene);

		vec3 gPosition = GetTranslate(nodeTransform); // Collision object center is node center
		if (!forceTrans)
			getObject()->collisionObject->setTranslate(gPosition, positionBefore);
		else
			getObject()->collisionObject->initTranslate(gPosition);
	} 
	if (rotate) {
		pushToUpdate(scene);

		vec3 gRotation = vec3(getObject()->anglex, getObject()->angley, getObject()->anglez);
		if(!forceTrans)
			getObject()->collisionObject->setRotate(gRotation, rotationBefore);
		else {
			vec4 gQuat = MatrixToQuat(getObject()->boundRotateMat);
			getObject()->collisionObject->initRotate(gQuat);
		}
	}
}

void AnimationNode::scaleNodeObject(Scene* scene, float sx, float sy, float sz) {
	AnimationObject* object = getObject();
	object->setSize(sx, sy, sz);

	pushToUpdate(scene);
}
