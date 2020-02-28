#include "frameMgr.h"

FrameMgr::FrameMgr() {
	frames.clear();
	datas = NULL;
	animCount = 0;
}

FrameMgr::~FrameMgr() {
	for (uint i = 0; i < frames.size(); ++i)
		delete frames[i];
	frames.clear();
	if (datas) free(datas); datas = NULL;
}

int FrameMgr::addFrame(AnimFrame* data) {
	if (data->frames.size() <= 0) return -1;

	int boneCount = data->frames[0]->boneCount, imgWidth = boneCount * 3;
	int frameCount = data->frames.size(), imgHeight = frameCount;
	float* texData = (float*)malloc(imgWidth * imgHeight * 4 * sizeof(float));

	for (int f = 0; f < frameCount; ++f) {
		Frame* frame = data->frames[f];
		memcpy(texData + f * boneCount * 12, frame->data, boneCount * 12 * sizeof(float));
	}

	uint curTex = frames.size();
	frames.push_back(new Texture2D(imgWidth, imgHeight, TEXTURE_TYPE_ANIME, FLOAT_PRE, 4, false, texData));
	free(texData);
	return curTex;
}

void FrameMgr::addAnimation(Animation* anim) {
	for (uint i = 0; i < anim->animCount; ++i) {
		int curFrame = addFrame(anim->animFrames[i]);
		anim->setFrameIndex(i, curFrame);
	}
}

void FrameMgr::init() {
	animCount = frames.size();
	datas = (u64*)malloc(animCount * sizeof(u64));
	for (uint i = 0; i < animCount; ++i)
		datas[i] = frames[i]->hnd;
}