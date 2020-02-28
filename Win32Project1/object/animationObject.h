/*
 * animationObject.h
 *
 *  Created on: 2017-6-25
 *      Author: a
 */

#ifndef ANIMATIONOBJECT_H_
#define ANIMATIONOBJECT_H_

#include "object.h"
#include "../animation/animation.h"

class AnimationObject: public Object {
private:
	virtual void vertexTransform();
	virtual void normalTransform();
public:
	float anglex,angley,anglez;
	Animation* animation;
	int aid, fid;
public:
	AnimationObject(Animation* anim);
	AnimationObject(const AnimationObject& rhs);
	virtual ~AnimationObject();
	virtual AnimationObject* clone();
	virtual void setPosition(float x, float y, float z);
	virtual void setRotation(float ax, float ay, float az);
	virtual void setSize(float sx, float sy, float sz);
	void setCurAnim(int aid);
};


#endif /* ANIMATIONOBJECT_H_ */
