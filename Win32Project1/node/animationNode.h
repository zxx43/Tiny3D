/*
 * animationNode.h
 *
 *  Created on: 2017-6-24
 *      Author: a
 */

#ifndef ANIMATIONNODE_H_
#define ANIMATIONNODE_H_

#include "node.h"
#include "../object/animationObject.h"

class AnimationNode: public Node {
private:
	Animation* animation;
private:
	vec3 positionBefore;
public:
	AnimationNode(const vec3& boundingSize);
	virtual ~AnimationNode();
	virtual void prepareDrawcall() { needCreateDrawcall = false; }
	virtual void updateRenderData() {}
	virtual void updateDrawcall() { needUpdateDrawcall = false; }
	void setAnimation(Scene* scene, Animation* anim);
	AnimationObject* getObject();
	void animate(float velocity);
	virtual void translateNode(Scene* scene, float x, float y, float z); // Local translate
	void translateNodeAtWorld(Scene* scene, float x, float y, float z); // Global translate (used in collision feedback)
	void translateNodeCenterAtWorld(Scene* scene, const vec3& nowWorldCenter);
	void rotateNodeObject(Scene* scene, float ax, float ay, float az);
	void rotateNodeAtWorld(Scene* scene, const vec4& quat);
	void doUpdateNodeTransform(Scene* scene, bool translate, bool rotate, bool forceTrans); // Update node transform & it's collision object
	void scaleNodeObject(Scene* scene, float sx, float sy, float sz);
};


#endif /* ANIMATIONNODE_H_ */
