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
	bool needUpdateNode;
public:
	AnimationNode(const vec3& boundingSize);
	virtual ~AnimationNode();
	virtual void prepareDrawcall();
	virtual void updateRenderData();
	virtual void updateDrawcall();
	void setAnimation(Scene* scene, Animation* anim);
	AnimationObject* getObject();
	void animate(float velocity);
	virtual void translateNode(Scene* scene, float x, float y, float z);
	void translateNodeCenterAtWorld(Scene* scene, float x, float y, float z);
	void rotateNodeObject(Scene* scene, float ax, float ay, float az);
	void setUpdate(bool need) { needUpdateNode = need; }
};


#endif /* ANIMATIONNODE_H_ */
