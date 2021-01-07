#ifndef FRAME_MGR_H_
#define FRAME_MGR_H_

#include "animation.h"
#include "../texture/texture2d.h"

class FrameMgr {
public:
	std::vector<Texture2D*> frames;
	std::map<std::string, int> frameIndex;
	u64* datas;
	uint animCount;
public:
	FrameMgr();
	~FrameMgr();
	void addAnimationData(AnimFrame* data, Animation* anim);
	void readAnimationData(const char* path, AnimFrame* animation);
	void init();
private:
	int addFrame(AnimFrame* data);
};

#endif
