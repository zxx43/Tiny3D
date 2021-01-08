#ifndef ANIMATION_DATA_H_
#define ANIMATION_DATA_H_

#include "../render/dataBuffer.h"
#include "../animation/animation.h"
#include "../object/animationObject.h"
#include "../constants/constants.h"

class AnimationDrawcall;

class AnimationData: public DataBuffer {
public:
	byte* boneids;
	half* weights;
	int animId;
	int animCount;
	buff* transformsFull;
public:
	AnimationData(Animation* anim, int maxCount);
	virtual ~AnimationData();
public:
	virtual void releaseDatas();
	void resetAnims() { animCount = 0; }
	void addAnimObject(Object* object);
};
#endif
