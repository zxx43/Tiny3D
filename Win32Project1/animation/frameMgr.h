#ifndef FRAME_MGR_H_
#define FRAME_MGR_H_

#include "animation.h"
#include "../texture/texture2d.h"

class FrameMgr {
public:
	std::vector<Texture2D*> frames;
	u64* datas;
	uint animCount;
public:
	FrameMgr();
	~FrameMgr();
	void addAnimation(Animation* anim);
	void init();
private:
	int addFrame(AnimFrame* data);
};

#endif
