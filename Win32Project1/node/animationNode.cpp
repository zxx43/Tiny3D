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
	translateNode(scene, dPosition.x, dPosition.y, dPosition.z);
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
	position = vec3(x, y, z);
	updateNodeTransform();
	
	pushToUpdate(scene);

	vec3 gPosition = GetTranslate(nodeTransform); // Collision object center is node center
	// todo update collision object
	//btTransform trans;
	//getObject()->collisionObject->getMotionState()->getWorldTransform(trans);
	//trans.setOrigin(gPosition);
	//getObject()->collisionObject->getMotionState()->setWorldTransform(trans);
}

void AnimationNode::rotateNodeObject(Scene* scene, float ax, float ay, float az) {
	AnimationObject* object = getObject();
	object->setRotation(ax, ay, az);

	pushToUpdate(scene);

	vec4 gQuat = MatrixToQuat(object->boundRotateMat);
	// todo update collision object
	//btTransform trans;
	//getObject()->collisionObject->getMotionState()->getWorldTransform(trans);
	//trans.setRotation(gQuat);
	//getObject()->collisionObject->getMotionState()->setWorldTransform(trans);
}

void AnimationNode::scaleNodeObject(Scene* scene, float sx, float sy, float sz) {
	AnimationObject* object = getObject();
	object->setSize(sx, sy, sz);

	pushToUpdate(scene);
}
