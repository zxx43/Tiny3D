#include "animationNode.h"
#include "../render/animationDrawcall.h"
#include "../util/util.h"

AnimationNode::AnimationNode(const VECTOR3D& boundingSize):
		Node(VECTOR3D(0, 0, 0), boundingSize) {
	animation = NULL;
	uTransformMatrix = new MATRIX4X4();
	uNormalMatrix = new MATRIX4X4();
	type = TYPE_ANIMATE;
}

AnimationNode::~AnimationNode() {}

void AnimationNode::setAnimation(Animation* anim) {
	animation = anim;
	addObject(new AnimationObject(animation));
}

void AnimationNode::prepareDrawcall() {
	if(drawcall) delete drawcall;
	drawcall = new AnimationDrawcall(animation);
	drawcall->setSide(singleSide);
	needCreateDrawcall = false;
}

void AnimationNode::updateDrawcall(int pass) {
	needUpdateDrawcall = false;
}

void AnimationNode::updateRenderData(Camera* camera, int pass) {
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
	MATRIX4X4 transform = nodeTransform * objects[0]->localTransformMatrix;
	MATRIX4X4 nTransform = objects[0]->normalMatrix;
	for (uint m = 0; m < 16; m++) {
		uTransformMatrix->entries[m] = transform.entries[m];
		uNormalMatrix->entries[m] = nTransform.entries[m];
	}

	VECTOR4D final4 = nodeTransform * VECTOR4D(0, 0, 0, 1);
	float invw = 1.0 / final4.w;
	VECTOR3D final3(final4.x * invw, final4.y * invw, final4.z * invw);
	boundingBox->update(final3);

	Node* superior = parent;
	while (superior) {
		superior->updateBounding();
		superior = superior->parent;
	}
}

void AnimationNode::translateNodeCenterAtWorld(float x, float y, float z) {
	VECTOR3D beforeWorldCenter = boundingBox->position;
	VECTOR3D offset = VECTOR3D(x, y, z) - beforeWorldCenter;
	translateNode(position.x + offset.x, position.y + offset.y, position.z + offset.z);
}

void AnimationNode::rotateNodeObject(float ax, float ay, float az) {
	AnimationObject* object = (AnimationObject*)objects[0];
	object->setRotation(ax, ay, az);

	MATRIX4X4 nTransform = object->normalMatrix;
	for (uint n = 0; n < 16; n++)
		uNormalMatrix->entries[n] = nTransform.entries[n];
}

