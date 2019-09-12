#include "animationNode.h"
#include "../render/animationDrawcall.h"
#include "../util/util.h"
#include "../scene/scene.h"
#include <string.h>

AnimationNode::AnimationNode(const vec3& boundingSize):
		Node(vec3(0, 0, 0), boundingSize) {
	animation = NULL;
	uTransformMatrix = new mat4();
	uNormalMatrix = new mat4();
	type = TYPE_ANIMATE;
}

AnimationNode::~AnimationNode() {}

void AnimationNode::setAnimation(Scene* scene, Animation* anim) {
	animation = anim;
	addObject(scene, new AnimationObject(animation));
}

void AnimationNode::prepareDrawcall() {
	if(drawcall) delete drawcall;
	drawcall = new AnimationDrawcall(animation);
	needCreateDrawcall = false;
}

void AnimationNode::updateDrawcall() {
	needUpdateDrawcall = false;
}

void AnimationNode::updateRenderData() {
}

void AnimationNode::animate(int animIndex,long startTime,long currentTime) {
	animation->bonesTransform(animIndex, (float)(currentTime - startTime)*0.001f);
}

AnimationObject* AnimationNode::getObject() {
	if (objects.size() > 0)
		return (AnimationObject*)objects[0];
	return NULL;
}

void AnimationNode::translateNode(float x,float y,float z) {
	position.x=x; position.y=y; position.z=z;

	recursiveTransform(nodeTransform);
	mat4 transform = nodeTransform * objects[0]->localTransformMatrix;
	mat4 nTransform = objects[0]->normalMatrix;
	memcpy(uTransformMatrix->entries, transform.entries, 16 * sizeof(float));
	memcpy(uNormalMatrix->entries, nTransform.entries, 16 * sizeof(float));

	vec4 final4 = nodeTransform * vec4(0, 0, 0, 1);
	float invw = 1.0 / final4.w;
	vec3 final3(final4.x * invw, final4.y * invw, final4.z * invw);
	boundingBox->update(final3);

	Node* superior = parent;
	while (superior) {
		superior->updateBounding();
		superior = superior->parent;
	}
}

void AnimationNode::translateNodeCenterAtWorld(float x, float y, float z) {
	vec3 beforeWorldCenter = boundingBox->position;
	vec3 offset = vec3(x, y, z) - beforeWorldCenter;
	translateNode(position.x + offset.x, position.y + offset.y, position.z + offset.z);
}

void AnimationNode::rotateNodeObject(float ax, float ay, float az) {
	AnimationObject* object = (AnimationObject*)objects[0];
	object->setRotation(ax, ay, az);
	memcpy(uNormalMatrix->entries, object->normalMatrix.entries, 16 * sizeof(float));
}

