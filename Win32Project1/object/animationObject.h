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
	vec4 quat;
	Animation* animation;
	int aid, fid;
public:
	int defaultAid;
	bool loop, playOnce, moving, animEnd;
	float time, curFrame;
public:
	AnimationObject(Animation* anim);
	AnimationObject(const AnimationObject& rhs);
	virtual ~AnimationObject();
	virtual AnimationObject* clone();
	virtual void setPosition(float x, float y, float z);
	virtual void setRotation(float ax, float ay, float az);
	virtual void setSize(float sx, float sy, float sz);
	void setRotation(const vec4& q);
	bool setCurAnim(int aid, bool once);
	int getCurAnim() { return aid; }
	void resetTime() { time = 0.0; }
	float getTime() { return time; }
	void setLoop(bool lp) { loop = lp; }
	bool isLoop() { return loop; }
	void setPlayOnce(bool once) { playOnce = once; }
	bool isPlayOnce() { return playOnce; }
	void setMoving(bool mv) { moving = mv; }
	bool isMoving() { return moving; }
	void setEnd(bool ed) { animEnd = ed; }
	bool isEnd() { return animEnd; }
	void setDefaultAnim(int aid) { defaultAid = aid; }
	bool isDefaultAnim() { return aid == defaultAid; }
	void animate(float velocity);
	float getCurFrame() { return curFrame; }
};


#endif /* ANIMATIONOBJECT_H_ */
